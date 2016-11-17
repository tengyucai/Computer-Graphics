-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

floor = gr.material({0.3, 0.3, 0.3}, {0.8, 0.8, 0.4}, 90)
gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 50)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 50)
silver = gr.material({0.75, 0.75, 0.75}, {1, 1, 1}, 128)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)

scene = gr.node('scene')
scene:rotate('X', 5)
scene:translate(6, -2, -15)

-- the arc
arc = gr.node('arc')
scene:add_child(arc)
arc:translate(0,0,-10)
arc:rotate('Y', 60)

p1 = gr.cube('p1')
arc:add_child(p1)
p1:set_material(gold)
p1:scale(0.8, 4, 0.8)
p1:translate(-2.4, 0, -0.4)

p2 = gr.cube('p2')
arc:add_child(p2)
p2:set_material(gold)
p2:scale(0.8, 4, 0.8)
p2:translate(1.6, 0, -0.4)

s = gr.sphere('s')
arc:add_child(s)
s:set_material(gold)
s:scale(4, 0.6, 0.6)
s:translate(0, 4, 0)

-- the floor
plane = gr.mesh( 'plane', 'Assets/plane.obj' )
scene:add_child(plane)
plane:set_material(floor)
plane:scale(30, 30, 30)

plane2 = gr.mesh( 'plane', 'Assets/plane.obj' )
scene:add_child(plane2)
plane2:set_material(mat2)
plane2:scale(300, 300, 300)
plane2:rotate('X', 90)
plane2:translate(0, 0, -50)

-- sphere
poly = gr.mesh( 'poly', 'Assets/dodeca.obj' )
scene:add_child(poly)
poly:translate(-2, 1.618034, 0)
poly:set_material(blue)

-- steldodec = gr.mesh( 'dodec', 'Assets/smstdodeca.obj' )
-- steldodec:set_material(gold)
-- scene:add_child(steldodec)
-- steldodec:scale(0.6, 0.6, 0.6)
-- steldodec:translate(100, -100, -40)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'sample.png', 512, 512, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})
