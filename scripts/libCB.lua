--[[
    @author: chenghua Wang
    @filen: libCB.lua
    @date: Nov 15, 2022
    @brief: This file is a lib for all lua script that need covalent bound
    backend to use. Include some trivial things.
]]

-- cretae the namespace using lua table.
F = {};
Op = {};
Cb = {};
Cb['F'] = F;
Cb['Op'] = Op;

function Cb.F.PackedCellsToVec(...)
    local nums = select('#', ...);
    if nums < 1 then
        return Cb.F.__cpp_packedAsVec();
    end
    local ans = Cb.F.__cpp_packedAsVec(select(1, ...));
    for i = 2, select('#', ...) do
        ans:add(select(i, ...));
    end
    return ans;
end

function Cb.F.createKVCell(graph, value, celltype)
    if graph == nil or value == nil then
        print("[ CB engine Error ] when execute Cb.F.createKVCell, @param(graph) and @param(value) should not be nil;");
        return nil;
    end
    if celltype == nil then
        return graph:createKVCell(value);
    end
    return graph:createKVCell(value, celltype);
end

function Cb.Op.combine()

end
