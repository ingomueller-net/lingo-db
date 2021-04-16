#ifndef RelAlg_RelAlgInterfaces
#define RelAlg_RelAlgInterfaces

#include "llvm/ADT/SmallPtrSet.h"
#include "mlir/Dialect/RelAlg/IR/RelAlgTypes.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/OpDefinition.h"
#include <mlir/Dialect/RelAlg/Attributes.h>
#include <mlir/IR/SymbolTable.h>

namespace mlir::relalg::detail {
Attributes getUsedAttributes(mlir::Operation* op);
Attributes getAvailableAttributes(mlir::Operation* op);
Attributes getFreeAttributes(mlir::Operation* op);
Attributes getCreatedAttributes(mlir::Operation* op);
bool isDependentJoin(mlir::Operation* op);

enum class BinaryOperatorType : unsigned char {
   None = 0,
   Union,
   Intersection,
   Except,
   CP,
   InnerJoin,
   SemiJoin,
   AntiSemiJoin,
   OuterJoin,
   FullOuterJoin,
   MarkJoin,
   LAST
};

template <class A, class B>
class CompatibilityTable {
   static constexpr size_t sizeA = static_cast<size_t>(A::LAST);
   static constexpr size_t sizeB = static_cast<size_t>(B::LAST);

   bool table[sizeA][sizeB];

   public:
   constexpr CompatibilityTable(std::initializer_list<std::pair<A, B>> l) : table() {
      for (auto item : l) {
         auto [a, b] = item;
         table[static_cast<size_t>(a)][static_cast<size_t>(b)] = true;
      }
   }
   constexpr bool contains(const A a, const B b) const {
      return table[static_cast<size_t>(a)][static_cast<size_t>(b)];
   }
};
constexpr CompatibilityTable<BinaryOperatorType, BinaryOperatorType> assoc{
   {BinaryOperatorType::Union, BinaryOperatorType::Union},
   {BinaryOperatorType::Intersection, BinaryOperatorType::Intersection},
   {BinaryOperatorType::Intersection, BinaryOperatorType::SemiJoin},
   {BinaryOperatorType::Intersection, BinaryOperatorType::AntiSemiJoin},
   {BinaryOperatorType::CP, BinaryOperatorType::CP},
   {BinaryOperatorType::CP, BinaryOperatorType::InnerJoin},
   {BinaryOperatorType::CP, BinaryOperatorType::SemiJoin},
   {BinaryOperatorType::CP, BinaryOperatorType::AntiSemiJoin},
   {BinaryOperatorType::CP, BinaryOperatorType::OuterJoin},
   {BinaryOperatorType::CP, BinaryOperatorType::MarkJoin},
   {BinaryOperatorType::InnerJoin, BinaryOperatorType::CP},
   {BinaryOperatorType::InnerJoin, BinaryOperatorType::InnerJoin},
   {BinaryOperatorType::InnerJoin, BinaryOperatorType::SemiJoin},
   {BinaryOperatorType::InnerJoin, BinaryOperatorType::AntiSemiJoin},
   {BinaryOperatorType::InnerJoin, BinaryOperatorType::OuterJoin},
   {BinaryOperatorType::InnerJoin, BinaryOperatorType::MarkJoin},

};
constexpr CompatibilityTable<BinaryOperatorType, BinaryOperatorType> lAsscom{
   {BinaryOperatorType::Union, BinaryOperatorType::Union},
   {BinaryOperatorType::Intersection, BinaryOperatorType::Intersection},
   {BinaryOperatorType::Intersection, BinaryOperatorType::SemiJoin},
   {BinaryOperatorType::Intersection, BinaryOperatorType::AntiSemiJoin},
   {BinaryOperatorType::Except, BinaryOperatorType::SemiJoin},
   {BinaryOperatorType::Except, BinaryOperatorType::AntiSemiJoin},
   {BinaryOperatorType::CP, BinaryOperatorType::CP},
   {BinaryOperatorType::CP, BinaryOperatorType::InnerJoin},
   {BinaryOperatorType::CP, BinaryOperatorType::SemiJoin},
   {BinaryOperatorType::CP, BinaryOperatorType::AntiSemiJoin},
   {BinaryOperatorType::CP, BinaryOperatorType::OuterJoin},
   {BinaryOperatorType::CP, BinaryOperatorType::MarkJoin},
   {BinaryOperatorType::InnerJoin, BinaryOperatorType::CP},
   {BinaryOperatorType::InnerJoin, BinaryOperatorType::InnerJoin},
   {BinaryOperatorType::InnerJoin, BinaryOperatorType::SemiJoin},
   {BinaryOperatorType::InnerJoin, BinaryOperatorType::AntiSemiJoin},
   {BinaryOperatorType::InnerJoin, BinaryOperatorType::OuterJoin},
   {BinaryOperatorType::InnerJoin, BinaryOperatorType::MarkJoin},
   {BinaryOperatorType::SemiJoin, BinaryOperatorType::Intersection},
   {BinaryOperatorType::SemiJoin, BinaryOperatorType::Except},
   {BinaryOperatorType::SemiJoin, BinaryOperatorType::CP},
   {BinaryOperatorType::SemiJoin, BinaryOperatorType::InnerJoin},
   {BinaryOperatorType::SemiJoin, BinaryOperatorType::SemiJoin},
   {BinaryOperatorType::SemiJoin, BinaryOperatorType::AntiSemiJoin},
   {BinaryOperatorType::SemiJoin, BinaryOperatorType::OuterJoin},
   {BinaryOperatorType::SemiJoin, BinaryOperatorType::MarkJoin},
   {BinaryOperatorType::AntiSemiJoin, BinaryOperatorType::Intersection},
   {BinaryOperatorType::AntiSemiJoin, BinaryOperatorType::Except},
   {BinaryOperatorType::AntiSemiJoin, BinaryOperatorType::CP},
   {BinaryOperatorType::AntiSemiJoin, BinaryOperatorType::InnerJoin},
   {BinaryOperatorType::AntiSemiJoin, BinaryOperatorType::SemiJoin},
   {BinaryOperatorType::AntiSemiJoin, BinaryOperatorType::AntiSemiJoin},
   {BinaryOperatorType::AntiSemiJoin, BinaryOperatorType::OuterJoin},
   {BinaryOperatorType::AntiSemiJoin, BinaryOperatorType::MarkJoin},
   {BinaryOperatorType::OuterJoin, BinaryOperatorType::CP},
   {BinaryOperatorType::OuterJoin, BinaryOperatorType::InnerJoin},
   {BinaryOperatorType::OuterJoin, BinaryOperatorType::SemiJoin},
   {BinaryOperatorType::OuterJoin, BinaryOperatorType::AntiSemiJoin},
   {BinaryOperatorType::OuterJoin, BinaryOperatorType::OuterJoin},
   {BinaryOperatorType::OuterJoin, BinaryOperatorType::MarkJoin},
   {BinaryOperatorType::MarkJoin, BinaryOperatorType::CP},
   {BinaryOperatorType::MarkJoin, BinaryOperatorType::InnerJoin},
   {BinaryOperatorType::MarkJoin, BinaryOperatorType::SemiJoin},
   {BinaryOperatorType::MarkJoin, BinaryOperatorType::AntiSemiJoin},
   {BinaryOperatorType::MarkJoin, BinaryOperatorType::OuterJoin},
   {BinaryOperatorType::MarkJoin, BinaryOperatorType::MarkJoin},

};
constexpr CompatibilityTable<BinaryOperatorType, BinaryOperatorType> rAsscom{
   {BinaryOperatorType::Union, BinaryOperatorType::Union},
   {BinaryOperatorType::Intersection, BinaryOperatorType::Intersection},
   {BinaryOperatorType::CP, BinaryOperatorType::CP},
   {BinaryOperatorType::CP, BinaryOperatorType::InnerJoin},
};

BinaryOperatorType getBinaryOperatorType(Operation* op);
bool binaryOperatorIs(const CompatibilityTable<BinaryOperatorType, BinaryOperatorType>& table, Operation* a, Operation* b);
bool isJoin(Operation* op);

void addPredicate(mlir::Operation* op, std::function<mlir::Value(mlir::Value, mlir::OpBuilder& builder)> predicateProducer);
void initPredicate(mlir::Operation* op);

void inlineOpIntoBlock(mlir::Operation* vop, mlir::Operation* includeChildren, mlir::Operation* excludeChildren, mlir::Block* newBlock, mlir::BlockAndValueMapping& mapping);
}
class Operator;
#define GET_OP_CLASSES
#include "mlir/Dialect/RelAlg/IR/RelAlgOpsInterfaces.h.inc"

#endif // RelAlg_RelAlgInterfaces
