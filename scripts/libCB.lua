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

function Cb.Op.CombineOp(baseOpPtr, primaryKeys)
    local inputs = baseOpPtr.io.I; -- vector.
    local output = baseOpPtr.io.O; -- virtual table.
    -- this program is a default behavior of combine operation.
    if #inputs ~= #primaryKeys then
        print("[ CB engine Error ] when execute Cb.Op.CombineOp. #inputs ~= #primaryKeys");
        return;
    end
    local inputs_len = #inputs;
    local colum_size = 0;
    local keys = {};
    local visited = {};
    -- get the index of primaryKeys, and calculate all colum_size.
    for i = 1, inputs_len do
        colum_size = colum_size + #inputs[i];
        keys.add(inputs[i]:KeyBy(primaryKeys[i]));
        visited[i] = {};
        for j = 1, #inputs[i] do
            visited[i][j] = false;
        end
    end
    -- reunit the Header info of virtual table.
    -- loop all Keys' index. And combine all.
    for i = 1, #keys do
        local cpp_vec_row = {};
        -- some fancy logic.
        -- TODO. How std::map works in lua table state?
        -- push to output's table.
        output:pushRow(cpp_vec_row);
    end
end
