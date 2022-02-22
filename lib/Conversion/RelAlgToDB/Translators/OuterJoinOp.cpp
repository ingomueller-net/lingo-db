#include "mlir/Conversion/RelAlgToDB/HashJoinTranslator.h"
#include "mlir/Conversion/RelAlgToDB/Translator.h"
#include "mlir/Dialect/DB/IR/DBOps.h"
#include "mlir/Dialect/RelAlg/IR/RelAlgOps.h"
#include "mlir/Dialect/util/UtilOps.h"
#include <mlir/Conversion/RelAlgToDB/NLJoinTranslator.h>
#include <mlir/IR/BlockAndValueMapping.h>

class OuterJoinTranslator : public mlir::relalg::JoinImpl {
   public:
   OuterJoinTranslator(mlir::relalg::OuterJoinOp innerJoinOp) : mlir::relalg::JoinImpl(innerJoinOp, innerJoinOp.right(), innerJoinOp.left()) {
      this->stopOnFlag = false;
   }

   virtual void handleLookup(mlir::Value matched, mlir::Value /*marker*/, mlir::relalg::TranslatorContext& context, mlir::OpBuilder& builder) override {
      translator->handlePotentialMatch(builder, context, matched, [&](mlir::OpBuilder& builder, mlir::relalg::TranslatorContext& context, mlir::relalg::TranslatorContext::AttributeResolverScope& scope) {
         translator->handleMapping(builder, context, scope);
         auto trueVal = builder.create<mlir::db::ConstantOp>(loc, mlir::db::BoolType::get(builder.getContext()), builder.getIntegerAttr(builder.getI64Type(), 1));
         builder.create<mlir::db::SetFlag>(loc, matchFoundFlag, trueVal);
      });
   }

   void beforeLookup(mlir::relalg::TranslatorContext& context, mlir::OpBuilder& builder) override {
      matchFoundFlag = builder.create<mlir::db::CreateFlag>(loc, mlir::db::FlagType::get(builder.getContext()));
   }
   void afterLookup(mlir::relalg::TranslatorContext& context, mlir::OpBuilder& builder) override {
      mlir::Value matchFound = builder.create<mlir::db::GetFlag>(loc, mlir::db::BoolType::get(builder.getContext()), matchFoundFlag);
      mlir::Value noMatchFound = builder.create<mlir::db::NotOp>(loc, mlir::db::BoolType::get(builder.getContext()), matchFound);
      translator->handlePotentialMatch(builder, context, noMatchFound, [&](mlir::OpBuilder& builder, mlir::relalg::TranslatorContext& context, mlir::relalg::TranslatorContext::AttributeResolverScope& scope) {
         translator->handleMappingNull(builder, context, scope);
      });
   }

   virtual ~OuterJoinTranslator() {}
};

class ReversedOuterJoinImpl : public mlir::relalg::JoinImpl {
   public:
   ReversedOuterJoinImpl(mlir::relalg::OuterJoinOp innerJoinOp) : mlir::relalg::JoinImpl(innerJoinOp, innerJoinOp.left(), innerJoinOp.right(), true) {
   }

   virtual void handleLookup(mlir::Value matched, mlir::Value markerPtr, mlir::relalg::TranslatorContext& context, mlir::OpBuilder& builder) override {
      translator->handlePotentialMatch(builder, context, matched, [&](mlir::OpBuilder& builder, mlir::relalg::TranslatorContext& context, mlir::relalg::TranslatorContext::AttributeResolverScope& scope) {
         translator->handleMapping(builder, context, scope);
      });
   }
   virtual void after(mlir::relalg::TranslatorContext& context, mlir::OpBuilder& builder) override {
      translator->scanHT(context, builder);
   }
   void handleScanned(mlir::Value marker, mlir::relalg::TranslatorContext& context, mlir::OpBuilder& builder) override {
      auto scope = context.createScope();
      auto zero = builder.create<mlir::arith::ConstantOp>(loc, marker.getType(), builder.getIntegerAttr(marker.getType(), 0));
      auto isZero = builder.create<mlir::arith::CmpIOp>(loc, mlir::arith::CmpIPredicate::eq, marker, zero);
      auto isZeroDB = builder.create<mlir::db::TypeCastOp>(loc, mlir::db::BoolType::get(builder.getContext()), isZero);
      translator->handlePotentialMatch(builder, context, isZeroDB, [&](mlir::OpBuilder& builder, mlir::relalg::TranslatorContext& context, mlir::relalg::TranslatorContext::AttributeResolverScope& scope) {
         translator->handleMappingNull(builder, context, scope);
      });
   }

   virtual ~ReversedOuterJoinImpl() {}
};

std::shared_ptr<mlir::relalg::JoinImpl> mlir::relalg::Translator::createOuterJoinImpl(mlir::relalg::OuterJoinOp joinOp, bool reversed) {
   return reversed ? (std::shared_ptr<mlir::relalg::JoinImpl>) std::make_shared<ReversedOuterJoinImpl>(joinOp) : (std::shared_ptr<mlir::relalg::JoinImpl>) std::make_shared<OuterJoinTranslator>(joinOp);
}