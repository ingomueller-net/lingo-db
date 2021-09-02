import pymlirdb
from sql2mlir.mlir import Function, DBType
import pyarrow as pa
supplier=pa.ipc.open_file(pa.OSFile('../../resources/data/tpch-1/supplier.arrow')).read_all()
lineitem=pa.ipc.open_file(pa.OSFile('../../resources/data/tpch-1/lineitem.arrow')).read_all()
orders=pa.ipc.open_file(pa.OSFile('../../resources/data/tpch-1/orders.arrow')).read_all()
nation=pa.ipc.open_file(pa.OSFile('../../resources/data/tpch-1/nation.arrow')).read_all()

pymlirdb.load_tables({"supplier":supplier,"lineitem":lineitem,"orders":orders,"nation":nation})

def count_delayed_orders_for_supplier(suppkey):
    items = pymlirdb.read_table("lineitem")
    items = items[items["l_suppkey"]==suppkey]
    orders = pymlirdb.read_table("orders")
    orders = orders[orders["o_orderstatus"] == 'F']
    orders = orders.join(items, on = (orders["o_orderkey"] == items["l_orderkey"]), how="left")
    orders["delayed"] = is_delayed(orders["o_orderkey"], suppkey)
    orders = orders[orders["delayed"] == True]
    return orders.count()

def is_delayed(orderkey, suppkey):
    containsDelayed = False
    containsOther = False
    onlyDelayed = True
    items = pymlirdb.read_table("lineitem")
    items = items[items["l_orderkey"] == orderkey]
    for item in items:
        if item["l_suppkey"] == suppkey:
            if item["l_receiptdate"] > item["l_commitdate"]:
                containsDelayed = True
        else:
            containsOther = True
            if item["l_receiptdate"] > item["l_commitdate"]:
                onlyDelayed = False
    return containsDelayed and containsOther and onlyDelayed

pymlirdb.registerFunction(Function("count_delayed_orders_for_supplier",[DBType("int",["64"])],DBType("int",["64"]),count_delayed_orders_for_supplier))
pymlirdb.registerFunction(Function("is_delayed",[DBType("int",["64"]),DBType("int",["64"])],DBType("bool"),is_delayed))

df = pymlirdb.query("""
                        select s_name, count_delayed_orders_for_supplier(s_suppkey) as numwait
                        from supplier, nation
                        where s_nationkey=n_nationkey
                        and n_name='SAUDI ARABIA'
                        order by numwait desc 
                    """)
print(df.to_pandas())