#include "mlir/Conversion/RelAlgToDB/HashJoinTranslator.h"
#include "mlir/Conversion/RelAlgToDB/Translator.h"
#include "mlir/Dialect/RelAlg/IR/RelAlgOps.h"
#include "mlir/Dialect/util/UtilOps.h"
#include <mlir/Conversion/RelAlgToDB/NLJoinTranslator.h>
#include <mlir/IR/BlockAndValueMapping.h>

class NLInnerJoinTranslator : public mlir::relalg::JoinImpl {
   public:
   NLInnerJoinTranslator(mlir::relalg::InnerJoinOp crossProductOp) : mlir::relalg::JoinImpl(crossProductOp, crossProductOp.left(), crossProductOp.right()) {}

   virtual void handleLookup(mlir::Value matched, mlir::Value /*marker*/, mlir::relalg::TranslatorContext& context, mlir::OpBuilder& builder) override {
      translator->handlePotentialMatch(builder, context, matched);
   }
   virtual ~NLInnerJoinTranslator() {}
};

std::shared_ptr<mlir::relalg::JoinImpl> mlir::relalg::Translator::createInnerJoinImpl(mlir::relalg::InnerJoinOp joinOp) {
   return std::make_shared<NLInnerJoinTranslator>(joinOp);
}