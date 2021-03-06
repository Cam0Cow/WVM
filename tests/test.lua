local tmpf = os.tmpname()
local success = true

for line in io.lines 'tests/tests.txt' do
	if line ~= '' and line:sub(1,1) ~= '#' then
		local tbl = {}
		for k in line:gmatch '%S+' do
			table.insert(tbl, k)
		end
		success = os.execute('./bin/vm > '.. tmpf .. ' tests/' .. tbl[1]) -- run vm with filename
		local f = io.open(tmpf, 'r')
		local l = f:read()
		if not l then l = '' end
		if l == tbl[2] then
			print (tbl[1] .. ' successful: ' .. l .. ' = ' .. tbl[2])
		else
			print (tbl[1] .. ' failed: ' .. l .. ' =/= ' .. tbl[2])
			success = false
		end
		io.stderr:write(string.rep('=', 60) .. '\n')
		f.close()
		os.execute("rm " .. tmpf)
	end
end

if not success then
  error '' -- hacky way to signal a failure
end
