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
        return Cb.F.__cpp_packedCellAsVec();
    end
    local ans = Cb.F.__cpp_packedCellAsVec(select(1, ...));
    for i = 2, select('#', ...) do
        ans:add(select(i, ...));
    end
    return ans;
end

function Cb.F.PackedStringToVec(...)
    local nums = select('#', ...);
    local ans = Cb.F.__cpp_packedStringAsVec();
    for i = 1, nums do
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

function Cb.F.value(_cpp_value)
    if _cpp_value:isFloat() then
        return _cpp_value:asFloat();
    elseif _cpp_value:isInt() then
        return _cpp_value:asInt();
    elseif _cpp_value:isULL() then
        return _cpp_value:asULL();
    elseif _cpp_value:isDataTime() or _cpp_value:isTime() or _cpp_value:isString() then
        return _cpp_value:asString();
    elseif _cpp_value:isDouble() then
        return _cpp_value:asDouble();
    end
end

function Cb.Op.CombineOp(baseOpPtr, primaryKeys)
    local inputs = baseOpPtr.io.I; -- vector.
    local output = baseOpPtr.io.O; -- virtual table.
    -- this program is a default behavior of combine operation.
    if #inputs ~= #primaryKeys then
        print("[ CB engine Error ] when execute Cb.Op.CombineOp. #inputs ~= #primaryKeys");
        return;
    end
    row = inputs[1]:getShape()[0];
    col = inputs[1]:getShape()[1];
    print(inputs[1]:colNameAt(0), inputs[1]:colNameAt(1), inputs[1]:colNameAt(2), inputs[1]:colNameAt(3));
    for i = 1, row do
        print(inputs[1]:atPtrRef(i - 1, 0):asInt(), inputs[1]:atPtrRef(i - 1, 1):asString(),
            inputs[1]:atPtrRef(i - 1, 2):asString(), inputs[1]:atPtrRef(i - 1, 3):asString());
    end
    output:resetShapeH(Cb.F.makeShapeFull(1, 1));
    output:setInfoAt(0, inputs[1]:getInfoAt(0));
    output:setPtrAt(0, 0, ThisGraph:createKVCell(1.2));
    print(output:atPtrRef(0, 0):asInt());
end
