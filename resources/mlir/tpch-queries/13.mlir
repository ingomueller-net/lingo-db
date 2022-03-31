module {
  func @main() -> !dsa.table {
    %0 = relalg.basetable @customer  {table_identifier = "customer"} columns: {c_acctbal => @c_acctbal({type = !db.decimal<15, 2>}), c_address => @c_address({type = !db.string}), c_comment => @c_comment({type = !db.string}), c_custkey => @c_custkey({type = i32}), c_mktsegment => @c_mktsegment({type = !db.string}), c_name => @c_name({type = !db.string}), c_nationkey => @c_nationkey({type = i32}), c_phone => @c_phone({type = !db.string})}
    %1 = relalg.basetable @orders  {table_identifier = "orders"} columns: {o_clerk => @o_clerk({type = !db.string}), o_comment => @o_comment({type = !db.string}), o_custkey => @o_custkey({type = i32}), o_orderdate => @o_orderdate({type = !db.date<day>}), o_orderkey => @o_orderkey({type = i32}), o_orderpriority => @o_orderpriority({type = !db.string}), o_orderstatus => @o_orderstatus({type = !db.char<1>}), o_shippriority => @o_shippriority({type = i32}), o_totalprice => @o_totalprice({type = !db.decimal<15, 2>})}
    %2 = relalg.outerjoin @oj0 %0, %1 (%arg0: !relalg.tuple){
      %7 = relalg.getcol %arg0 @customer::@c_custkey : i32
      %8 = relalg.getcol %arg0 @orders::@o_custkey : i32
      %9 = db.compare eq %7 : i32, %8 : i32
      %10 = relalg.getcol %arg0 @orders::@o_comment : !db.string
      %11 = db.constant("%special%requests%") : !db.string
      %12 = db.compare like %10 : !db.string, %11 : !db.string
      %13 = db.not %12 : i1
      %14 = db.and %9, %13 : i1, i1
      relalg.return %14 : i1
    }  mapping: {@o_orderkey({type = !db.nullable<i32>})=[@orders::@o_orderkey], @o_custkey({type = !db.nullable<i32>})=[@orders::@o_custkey], @o_orderstatus({type = !db.nullable<!db.char<1>>})=[@orders::@o_orderstatus], @o_totalprice({type = !db.nullable<!db.decimal<15, 2>>})=[@orders::@o_totalprice], @o_orderdate({type = !db.nullable<!db.date<day>>})=[@orders::@o_orderdate], @o_orderpriority({type = !db.nullable<!db.string>})=[@orders::@o_orderpriority], @o_clerk({type = !db.nullable<!db.string>})=[@orders::@o_clerk], @o_shippriority({type = !db.nullable<i32>})=[@orders::@o_shippriority], @o_comment({type = !db.nullable<!db.string>})=[@orders::@o_comment]}
    %3 = relalg.aggregation @aggr0 %2 [@customer::@c_custkey] (%arg0: !relalg.tuplestream,%arg1: !relalg.tuple){
      %7 = relalg.aggrfn count @oj0::@o_orderkey %arg0 : i64
      %8 = relalg.addcol %arg1, @tmp_attr0({type = i64}) %7
      relalg.return %8 : !relalg.tuple
    }
    %4 = relalg.aggregation @aggr1 %3 [@aggr0::@tmp_attr0] (%arg0: !relalg.tuplestream,%arg1: !relalg.tuple){
      %7 = relalg.count %arg0
      %8 = relalg.addcol %arg1, @tmp_attr1({type = i64}) %7
      relalg.return %8 : !relalg.tuple
    }
    %5 = relalg.sort %4 [(@aggr1::@tmp_attr1,desc),(@aggr0::@tmp_attr0,desc)]
    %6 = relalg.materialize %5 [@aggr0::@tmp_attr0,@aggr1::@tmp_attr1] => ["c_count", "custdist"] : !dsa.table
    return %6 : !dsa.table
  }
}
