#!/usr/local/bin/lua

local tmpf = os.tmpname()
local success = true

for line in io.lines 'tests/tests.txt' do
  local tbl = {}
  for k in line:gmatch '%S+' do
    table.insert(tbl, k)
  end
	success = os.execute('./bin/vm > '.. tmpf .. ' tests/' .. tbl[1]) -- run vm with filename
	local f = io.open(tmpf, 'r')
	local l = f:read()
	if l == tbl[2] then
		print (tbl[1] .. ' successful: ' .. l .. ' = ' .. tbl[2])
	else
		print (tbl[1] .. ' failed: ' .. l .. ' =/= ' .. tbl[2])
    success = false
	end
  io.stderr:write(string.rep('=', 60) .. '\n')
end

if not success then
  error '' -- hacky way to signal a failure
end
