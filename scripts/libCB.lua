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

function Cb.F.isStringEqual(s1, s2)
    return s1 == s2;
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

function Cb.F.setTableName(table, tableName)
    local col = table:getShape()[1];
    for i = 1, col do
        table:getInfoAt(i - 1):setTableName(tableName);
    end
end

function Cb.Op.CombineOp(baseOpPtr, primaryKeys, newTableName)
    local inputs = baseOpPtr.io.I; -- vector.
    local output = baseOpPtr.io.O; -- virtual table.
    -- this program is a default behavior of combine operation.

    if #inputs ~= #primaryKeys then
        print("[ CB engine Error ] when execute Cb.Op.CombineOp. #inputs ~= #primaryKeys");
        return;
    end

    for n = 1, #inputs do
        for r = 1, inputs[n]:getShape()[0] do
            for c = 1, inputs[n]:getShape()[1] do
                Cb.F.value(inputs[n]:atPtr(r - 1, c - 1))
            end
        end
    end

    local allColName = {}; -- default new primarykey is first in primaryKeys
    local allPrimaryKeysLocation = {};
    local primaryKeysPtr = 0;
    local otherKyesPtr = 0;
    for i = 1, #inputs do
        for j = 1, inputs[i]:getShape()[1] do
            if inputs[i]:colNameAt(j - 1) == primaryKeys[i] then
                allPrimaryKeysLocation[i] = j - 1;
            end
        end
    end

    for i = 1, #primaryKeys do
        allColName[i] = primaryKeys[i]; -- primaryKeys is in the front of allColName
        primaryKeysPtr = i;
    end
    otherKyesPtr = primaryKeysPtr + 1;

    for i = 1, inputs[1]:getShape()[1] do
        for j = 1, #primaryKeys do
            if Cb.F.isStringEqual(primaryKeys[j], inputs[1]:colNameAt(i - 1)) then
                goto continue;
            end
        end
        allColName[otherKyesPtr] = inputs[1]:colNameAt(i - 1);
        otherKyesPtr = otherKyesPtr + 1;
        ::continue::
    end

    for k = 2, #inputs do
        for i = 1, inputs[k]:getShape()[1] do
            for j = 1, #primaryKeys do
                if Cb.F.isStringEqual(primaryKeys[j], inputs[k]:colNameAt(i - 1)) then
                    goto continue;
                end
            end
            allColName[otherKyesPtr] = inputs[k]:colNameAt(i - 1);
            otherKyesPtr = otherKyesPtr + 1;
            ::continue::
        end
    end
    local outputColNum = otherKyesPtr - primaryKeysPtr;

    -- -- Get all keys, next step is to judge how many primaryKeys
    -- search for first primarykey index
    local indPrimaykeys = 0;
    for i = 0, inputs[1]:getShape()[1] - 1 do
        if inputs[1]:colNameAt(i) == primaryKeys[1] then
            indPrimaykeys = i;
            break;
        end
    end

    -- use first primarykey index to get correspondding column
    local tmpPrimryKeysCol = inputs[1]:getCol(indPrimaykeys);
    tmpPrimryKeysCol:resetShapeH(Cb.F.makeShapeFull(tmpPrimryKeysCol:getShape()[0], outputColNum));

    -- compare with other table, if there are other data, add in
    local tmpInd;
    for i = 2, #inputs do
        tmpInd = inputs[i]:keyBy(primaryKeys[i]); -- map
        for j = 1, #tmpInd do
            for k = 1, tmpPrimryKeysCol:getShape()[0] do -- virtual table
                if Cb.F.value(inputs[i]:atPtr(tmpInd[j], allPrimaryKeysLocation[i])) ==
                    Cb.F.value(tmpPrimryKeysCol:atPtr(k - 1, 0)) then
                    goto continue;
                end
            end
            tmpPrimryKeysCol:resetShapeH(Cb.F.makeShapeFull(tmpPrimryKeysCol:getShape()[0] + 1, outputColNum));
            tmpPrimryKeysCol:setPtrAt(tmpPrimryKeysCol:getShape()[0] - 1, 0,
                inputs[i]:atPtr(j - 1, allPrimaryKeysLocation[i]));
            ::continue::
        end
    end

    -- point to point
    local colPtrTail = 1;
    local colPtrHead = 1;
    local bufCol = nil;
    for i = 1, tmpPrimryKeysCol:getShape()[0] do -- loop in all primaykeys
        for j = 1, #inputs do -- loop in all table
            bufCol = inputs[j]:keyBy(allColName[j]) -- map index
            for n = 1, #bufCol do
                if Cb.F.value(inputs[j]:atPtr(bufCol[n], allPrimaryKeysLocation[j])) ==
                    Cb.F.value(tmpPrimryKeysCol:atPtr(i - 1, 0)) then -- if primarykey is same
                    colPtrTail = colPtrHead;
                    for c = 1, inputs[j]:getShape()[1] do
                        if c == allPrimaryKeysLocation[j] + 1 then
                            goto continue
                        end
                        tmpPrimryKeysCol:setPtrAt(i - 1, colPtrTail, inputs[j]:atPtr(bufCol[n], c - 1));
                        colPtrTail = colPtrTail + 1;
                        ::continue::
                    end
                end
            end
            colPtrHead = colPtrTail;
        end
        colPtrHead = 1;
    end

    local colPtr = 1;
    output:resetShapeH(Cb.F.makeShapeFull(tmpPrimryKeysCol:getShape()[0], outputColNum));
    for r = 1, output:getShape()[0] do
        for c = 1, output:getShape()[1] do
            output:setPtrAt(r - 1, c - 1, tmpPrimryKeysCol:atPtr(r - 1, c - 1));
            -- print(Cb.F.value(output:atPtr(r - 1, c - 1)))
        end
    end

    output:setInfoAt(0, inputs[1]:getInfoAt(allPrimaryKeysLocation[1]));
    for i = 1, #inputs do
        for j = 1, inputs[i]:getShape()[1] do
            if j == allPrimaryKeysLocation[i] + 1 then
                goto continue;
            end
            output:setInfoAt(colPtr, inputs[i]:getInfoAt(j - 1));
            colPtr = colPtr + 1;
            ::continue::
        end
    end

    Cb.F.setTableName(output, newTableName);

    -- for c = 1, output:getShape()[1] do
    --     print(c, output:colNameAt(c - 1))
    -- end

end
