node_vd_1 = ThisGraph:createVirtualDeviceNode(0);
node_vd_2 = ThisGraph:createVirtualDeviceNode(1);
node_vd_1:addQuery("SELECT * FROM runoob_tbl;");
node_vd_2:addQuery("SELECT * FROM runoob_tbl;");

node_vd_combine = ThisGraph:createCombineNode(Cb.F.PackedStringToVec("Key1", "Key2"));
node_vd_1:PointTo(Cb.F.refNode(node_vd_combine));
node_vd_2:PointTo(Cb.F.refNode(node_vd_combine));

ThisGraph:addCacheServer(ThisGraph:createRedisCachingNode(0));
