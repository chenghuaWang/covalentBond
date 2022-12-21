function testJudgeMethod(rowOfTable)
    if rowOfTable:atPtr(0, 0):isInt() then
        if Cb.F.value(rowOfTable:atPtr(0, 0)) < 10 then
            return true;
        end
    end
    return false;
end

function testModifyMethod(rowOfTable)
    rowOfTable:setPtrAt(0, 0, ThisGraph:createKVCell(Cb.F.value(rowOfTable:atPtr(0, 0)) + 1));
    return rowOfTable;
end

node_vd_1 = ThisGraph:createVirtualDeviceNode(0);
node_vd_2 = ThisGraph:createVirtualDeviceNode(1);
node_vd_1:addQuery("SELECT * FROM runoob_tbl;");
node_vd_2:addQuery("SELECT * FROM myel;");

node_vd_combine = ThisGraph:createCombineNode(Cb.F.PackedStringToVec("runoob_id", "runoob_id"), "NewTable");
node_vd_1:PointTo(Cb.F.refNode(node_vd_combine));
node_vd_2:PointTo(Cb.F.refNode(node_vd_combine));
node_vd_filter = ThisGraph:createFilterNode(testJudgeMethod, testModifyMethod);
node_vd_combine:PointTo(Cb.F.refNode(node_vd_filter))

ThisGraph:addCacheServer(ThisGraph:createRedisCachingNode(0));
