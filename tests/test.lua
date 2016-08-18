#!/usr/local/bin/lua

for line in io.lines 'tests/tests.txt' do
  local tbl = {}
  for k in line:gmatch '%S+' do
    table.insert(tbl, k)
  end

end
