#ifndef MLIR_CONVERSION_DBTOARROWSTD_COLLECTIONITERATION_H
#define MLIR_CONVERSION_DBTOARROWSTD_COLLECTIONITERATION_H
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/TypeRange.h"
#include "mlir/Transforms/DialectConversion.h"

namespace mlir::db {
class CollectionIterationImpl {
   public:
   virtual std::vector<Value> implementLoop(mlir::TypeRange iterArgTypes, mlir::TypeConverter& typeConverter, OpBuilder builder, mlir::ModuleOp parentModule, std::function<std::vector<Value>(ValueRange, OpBuilder)> bodyBuilder) = 0;
   virtual ~CollectionIterationImpl() {
   }
   static std::unique_ptr<mlir::db::CollectionIterationImpl> getImpl(mlir::Type collectionType, mlir::Value collection);
};

} // namespace mlir::db
#endif // MLIR_CONVERSION_DBTOARROWSTD_COLLECTIONITERATION_H
