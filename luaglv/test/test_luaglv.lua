--setup
glv1 = glv.glv.new()
create_window()
addglv_towindow(glv1)
fontgl.initialize()
font1 = fontgl.new("../../../test/ProFontWindows.ttf", 12, true)
font1:setColor(1., 1., 1., 1.)
print(font1)

columnViews = {}

w, h = glv1:getExtent()

text = {{"LuaGLV", "kicks", "bootay"},
		{"LuaGLV", "rocks", "on"},
		{"LuaGLV", "makes me", "hornay"},
		{"God,", "I need", "a woman"}}

function subviewDrawLabel(index1, index2, v)
	local w, h = v:getExtent()
	local length = font1:advance(text[index1][index2])
	font1:render((w-length)/2, h/2, 0, text[index1][index2])
end

for i=1, 4 do
	columnViews[i] = glv.view.new(w/4 * (i-1) + 10, h/2 - 100, 140, 200)
	glv1:add(columnViews[i])
	
	columnViews[i].MouseDrag = function(v, g)
									v:move(g:mouseDelta())
									
									for k, child in ipairs(v.children) do
										child.drawlabel = true
									end
								end
	
	columnViews[i].MouseUp = function(v, g)
									for k, child in ipairs(v.children) do
										child.drawlabel = false
									end
								end
	
	columnViews[i].children = {}
	for j=1, 3 do
		columnViews[i].children[j] = glv.view.new(10, 60 * (j-1) + 10, 
													columnViews[i]:getWidth() - 20, 40)
		columnViews[i]:add(columnViews[i].children[j])
		
		
		local drawText = function(v)
								subviewDrawLabel(i, j, v)
							end
		
		columnViews[i].children[j].drawlabel = false
		columnViews[i].children[j].Draw = function(v)
												if(v.drawlabel) then
													drawText(v)
												end
											end
	end
end



s1 = glv.slider.new(10, 10, 100, 20)
glv1:add(s1)
glv1.Draw3D = function(v, w, h)
				gl.Translate(0., 0., 2.)
				gl.Color(1., 0., 0., 1.)
				gl.Begin("QUADS")
					gl.Vertex(-1., 20., 0)
					gl.Vertex(1., 20., 0)
					gl.Vertex(1., -20., 0)
					gl.Vertex(-1., -20., 0)
				gl.End()
			end