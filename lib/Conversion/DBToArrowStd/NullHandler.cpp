#include "mlir/Conversion/DBToArrowStd/NullHandler.h"
#include <mlir/Dialect/DB/IR/DBType.h>
#include <mlir/Dialect/util/UtilOps.h>

mlir::Value mlir::db::NullHandler::getValue(Value v, Value operand) {
   Type type = v.getType();
   if (auto dbType = type.dyn_cast_or_null<mlir::db::DBType>()) {
      if (dbType.isNullable()) {
         TupleType tupleType = typeConverter.convertType(v.getType()).dyn_cast_or_null<TupleType>();
         auto unPackOp = builder.create<mlir::util::UnPackOp>(builder.getUnknownLoc(), tupleType.getTypes(), v);
         nullValues.push_back(unPackOp.vals()[0]);
         return unPackOp.vals()[1];
      } else {
         return operand ? operand : v;
      }
   } else {
      return operand ? operand : v;
   }
}
mlir::Value mlir::db::NullHandler::isNull() {
   if (nullValues.empty()) {
      return builder.create<arith::ConstantOp>(builder.getUnknownLoc(), builder.getI1Type(), builder.getIntegerAttr(builder.getI1Type(), 0));
   }
   Value isNull;
   if (nullValues.size() >= 1) {
      isNull = nullValues.front();
   }
   for (size_t i = 1; i < nullValues.size(); i++) {
      isNull = builder.create<arith::OrIOp>(builder.getUnknownLoc(), isNull.getType(), isNull, nullValues[i]);
   }
   return isNull;
};
mlir::Value mlir::db::NullHandler::combineResult(Value res) {
   auto i1Type = IntegerType::get(builder.getContext(), 1);
   if (nullValues.empty()) {
      return res;
   }
   Value isNull;
   if (nullValues.size() >= 1) {
      isNull = nullValues.front();
   }
   for (size_t i = 1; i < nullValues.size(); i++) {
      isNull = builder.create<arith::OrIOp>(builder.getUnknownLoc(), isNull.getType(), isNull, nullValues[i]);
   }
   return builder.create<mlir::util::PackOp>(builder.getUnknownLoc(), mlir::TupleType::get(builder.getContext(), {i1Type, res.getType()}), ValueRange({isNull, res}));
}