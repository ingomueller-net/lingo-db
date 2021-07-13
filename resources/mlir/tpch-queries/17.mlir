module @querymodule{
    func @main (%executionContext: !util.generic_memref<i8>)  -> !db.table{
        %1 = relalg.basetable @lineitem { table_identifier="lineitem", rows=600572 , pkey=["l_orderkey","l_linenumber"]} columns: {l_orderkey => @l_orderkey({type=!db.int<64>}),
            l_partkey => @l_partkey({type=!db.int<64>}),
            l_suppkey => @l_suppkey({type=!db.int<64>}),
            l_linenumber => @l_linenumber({type=!db.int<32>}),
            l_quantity => @l_quantity({type=!db.decimal<15,2>}),
            l_extendedprice => @l_extendedprice({type=!db.decimal<15,2>}),
            l_discount => @l_discount({type=!db.decimal<15,2>}),
            l_tax => @l_tax({type=!db.decimal<15,2>}),
            l_returnflag => @l_returnflag({type=!db.string}),
            l_linestatus => @l_linestatus({type=!db.string}),
            l_shipdate => @l_shipdate({type=!db.date<day>}),
            l_commitdate => @l_commitdate({type=!db.date<day>}),
            l_receiptdate => @l_receiptdate({type=!db.date<day>}),
            l_shipinstruct => @l_shipinstruct({type=!db.string}),
            l_shipmode => @l_shipmode({type=!db.string}),
            l_comment => @l_comment({type=!db.string})
        }
        %2 = relalg.basetable @part { table_identifier="part", rows=20000 , pkey=["p_partkey"]} columns: {p_partkey => @p_partkey({type=!db.int<64>}),
            p_name => @p_name({type=!db.string}),
            p_mfgr => @p_mfgr({type=!db.string}),
            p_brand => @p_brand({type=!db.string}),
            p_type => @p_type({type=!db.string}),
            p_size => @p_size({type=!db.int<32>}),
            p_container => @p_container({type=!db.string}),
            p_retailprice => @p_retailprice({type=!db.decimal<15,2>}),
            p_comment => @p_comment({type=!db.string})
        }
        %3 = relalg.crossproduct %1, %2
        %5 = relalg.selection %3(%4: !relalg.tuple) {
            %6 = relalg.getattr %4 @part::@p_partkey : !db.int<64>
            %7 = relalg.getattr %4 @lineitem::@l_partkey : !db.int<64>
            %8 = db.compare eq %6 : !db.int<64>,%7 : !db.int<64>
            %9 = relalg.getattr %4 @part::@p_brand : !db.string
            %10 = db.constant ("Brand#23") :!db.string
            %11 = db.compare eq %9 : !db.string,%10 : !db.string
            %12 = relalg.getattr %4 @part::@p_container : !db.string
            %13 = db.constant ("MED BOX") :!db.string
            %14 = db.compare eq %12 : !db.string,%13 : !db.string
            %15 = relalg.getattr %4 @lineitem::@l_quantity : !db.decimal<15,2>
            %16 = relalg.basetable @lineitem1 { table_identifier="lineitem", rows=600572 , pkey=["l_orderkey","l_linenumber"]} columns: {l_orderkey => @l_orderkey({type=!db.int<64>}),
                l_partkey => @l_partkey({type=!db.int<64>}),
                l_suppkey => @l_suppkey({type=!db.int<64>}),
                l_linenumber => @l_linenumber({type=!db.int<32>}),
                l_quantity => @l_quantity({type=!db.decimal<15,2>}),
                l_extendedprice => @l_extendedprice({type=!db.decimal<15,2>}),
                l_discount => @l_discount({type=!db.decimal<15,2>}),
                l_tax => @l_tax({type=!db.decimal<15,2>}),
                l_returnflag => @l_returnflag({type=!db.string}),
                l_linestatus => @l_linestatus({type=!db.string}),
                l_shipdate => @l_shipdate({type=!db.date<day>}),
                l_commitdate => @l_commitdate({type=!db.date<day>}),
                l_receiptdate => @l_receiptdate({type=!db.date<day>}),
                l_shipinstruct => @l_shipinstruct({type=!db.string}),
                l_shipmode => @l_shipmode({type=!db.string}),
                l_comment => @l_comment({type=!db.string})
            }
            %18 = relalg.selection %16(%17: !relalg.tuple) {
                %19 = relalg.getattr %17 @lineitem1::@l_partkey : !db.int<64>
                %20 = relalg.getattr %4 @part::@p_partkey : !db.int<64>
                %21 = db.compare eq %19 : !db.int<64>,%20 : !db.int<64>
                relalg.return %21 : !db.bool
            }
            %23 = relalg.aggregation @aggr1 %18 [] (%22 : !relalg.relation) {
                %24 = relalg.aggrfn avg @lineitem1::@l_quantity %22 : !db.decimal<15,2,nullable>
                relalg.addattr @aggfmname1({type=!db.decimal<15,2,nullable>}) %24
                relalg.return
            }
            %26 = relalg.map @map2 %23 (%25: !relalg.tuple) {
                %27 = db.constant ("0.2") :!db.decimal<15,2>
                %28 = relalg.getattr %25 @aggr1::@aggfmname1 : !db.decimal<15,2,nullable>
                %29 = db.mul %27 : !db.decimal<15,2>,%28 : !db.decimal<15,2,nullable>
                relalg.addattr @aggfmname2({type=!db.decimal<15,2,nullable>}) %29
                relalg.return
            }
            %30 = relalg.getscalar @map2::@aggfmname2 %26 : !db.decimal<15,2,nullable>
            %31 = db.compare lt %15 : !db.decimal<15,2>,%30 : !db.decimal<15,2,nullable>
            %32 = db.and %8 : !db.bool,%11 : !db.bool,%14 : !db.bool,%31 : !db.bool<nullable>
            relalg.return %32 : !db.bool<nullable>
        }
        %34 = relalg.aggregation @aggr2 %5 [] (%33 : !relalg.relation) {
            %35 = relalg.aggrfn sum @lineitem::@l_extendedprice %33 : !db.decimal<15,2,nullable>
            relalg.addattr @aggfmname1({type=!db.decimal<15,2,nullable>}) %35
            relalg.return
        }
        %37 = relalg.map @map4 %34 (%36: !relalg.tuple) {
            %38 = relalg.getattr %36 @aggr2::@aggfmname1 : !db.decimal<15,2,nullable>
            %39 = db.constant ("7.0") :!db.decimal<15,2>
            %40 = db.div %38 : !db.decimal<15,2,nullable>,%39 : !db.decimal<15,2>
            relalg.addattr @aggfmname2({type=!db.decimal<15,2,nullable>}) %40
            relalg.return
        }
        %41 = relalg.materialize %37 [@map4::@aggfmname2] => ["avg_yearly"] : !db.table
        return %41 : !db.table
    }
}

