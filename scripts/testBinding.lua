-- 1. build graph

node_vd_1 = ThisGraph:createVirtualDeviceNode(0);
node_vd_2 = ThisGraph:createVirtualDeviceNode(0);
node_vd_1:addQuery("SELECT * FROM runoob_tbl;");
node_vd_2:addQuery("SELECT * FROM runoob_tbl_2;");

node_vd_combine = ThisGraph:createCombineNode();
node_vd_1:PointTo(cb.F.refNode(node_vd_combine));
node_vd_2:PointTo(cb.F.refNode(node_vd_combine));

ThisGraph:setFinalNode(cb.F.refNode(node_vd_combine));

-- 2. override combine's function.

function CombineOpOverride(output, ...)
    -- output / ... for input
    -- here for your own process logic.
end

node_vd_combine:overrideFunc(CombineOpOverride);
