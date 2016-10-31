rootnode = gr.node('root')
rootnode:rotate('y', 0.0)
rootnode:scale( 0.2, 0.2, 0.2 )
rootnode:translate(0.0, 0.0, -1.0)

white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

node = gr.node('node')
rootnode:add_child(node)
--node:scale( 0.1, 0.1, 0.1 )

torso = gr.mesh('sphere', 'torso')
node:add_child(torso)
torso:set_material(white)
--torso:scale(1/0.1, 1/0.1, 1/0.1); 
torso:scale(0.85, 1.0, 0.8); 



node1 = gr.node('node1')
node:add_child(node1)
--node1:scale( 0.1, 0.1, 0.1 )
node1:translate(0.0, 0.5, 0.0)

joint1 = gr.joint('joint1',{-5, 0, 45}, {0, 0, 0})
node1:add_child(joint1)

shoulder = gr.mesh('sphere', 'shoulder')
joint1:add_child(shoulder)
--shoulder:scale(1/0.9, 1/1.0, 1/0.8);
shoulder:scale(0.6, 0.7, 0.6)
shoulder:translate(0.0, 0.1, 0)
shoulder:set_material(white)




node2 = gr.node('node2')
node:add_child(node2)
--node2:scale( 1/0.1, 1/0.1, 1/0.1 )
--node2:scale( 0.1, 0.1, 0.1 )
node2:translate(-0.3, -0.8, 0)

joint2 = gr.joint('joint2',{-70, 0, 30}, {0, 0, 0})
node2:add_child(joint2)

leftLeg = gr.mesh('sphere', 'leftLeg')
joint2:add_child(leftLeg)
--leftLeg:scale(1/0.9, 1/1.0, 1/0.8);
leftLeg:scale(0.3, 0.5, 0.3)
leftLeg:translate(0, -0.2, 0)
leftLeg:set_material(white)




node3 = gr.node('node3')
node:add_child(node3)
--node3:scale( 0.1, 0.1, 0.1 )
node3:translate(0.3, -0.8, 0)

joint3 = gr.joint('joint3',{-70, 0, 30}, {0, 0, 0})
node3:add_child(joint3)

rightLeg = gr.mesh('sphere', 'rightLeg')
joint3:add_child(rightLeg)
--rightLeg:scale(1/0.9, 1/1.0, 1/0.8);
rightLeg:scale(0.3, 0.5, 0.3)
rightLeg:translate(0, -0.2, 0)
rightLeg:set_material(white)




node4 = gr.node('node4')
joint1:add_child(node4)
--node4:scale( 0.1, 0.1, 0.1 )
node4:translate(-0.4, 0.6, 0.15)

joint4 = gr.joint('joint4',{-160, 0, 25}, {0, 0, 0})
node4:add_child(joint4)

leftUpperArm = gr.mesh('sphere', 'leftUpperArm')
joint4:add_child(leftUpperArm)
--leftUpperArm:scale(1/0.2, 1/0.2, 1/0.2);
leftUpperArm:scale(0.25, 0.6, 0.25)
leftUpperArm:rotate('z', -45)
leftUpperArm:translate(-0.35, -0.4, 0.0)
--leftUpperArm:translate(0, 0, 0)
leftUpperArm:set_material(white)



node5 = gr.node('node4')
joint1:add_child(node5)
--node4:scale( 0.1, 0.1, 0.1 )
node5:translate(0.4, 0.6, 0.15)

joint5 = gr.joint('joint5',{-160, 0, 25}, {0, 0, 0})
node5:add_child(joint5)

rightUpperArm = gr.mesh('sphere', 'rightUpperArm')
joint5:add_child(rightUpperArm)
--leftUpperArm:scale(1/0.2, 1/0.2, 1/0.2);
rightUpperArm:scale(0.25, 0.6, 0.25)
rightUpperArm:rotate('z', 45)
rightUpperArm:translate(0.35, -0.4, 0.0)
--leftUpperArm:translate(0, 0, 0)
rightUpperArm:set_material(white)



node6 = gr.node('node6')
joint4:add_child(node6)
--node6:scale( 0.1, 0.1, 0.1 )
node6:translate(-0.65, -0.7, 0)

joint6 = gr.joint('joint6',{-150, 0, 0}, {0, 0, 0})
node6:add_child(joint6)

leftLowerArm = gr.mesh('sphere', 'leftLowerArm')
joint6:add_child(leftLowerArm)
--leftLowerArm:scale(1/0.2, 1/0.2, 1/0.2);
leftLowerArm:scale(0.3, 0.65, 0.3)
leftLowerArm:rotate('z', -10)
leftLowerArm:translate(-0.1, -0.5, 0)
leftLowerArm:set_material(white)



node7 = gr.node('node7')
joint5:add_child(node7)
--node6:scale( 0.1, 0.1, 0.1 )
node7:translate(0.65, -0.7, 0)

joint7 = gr.joint('joint7',{-150, 0, 0}, {0, 0, 0})
node7:add_child(joint7)

rightLowerArm = gr.mesh('sphere', 'rightLowerArm')
joint7:add_child(rightLowerArm)
--leftLowerArm:scale(1/0.2, 1/0.2, 1/0.2);
rightLowerArm:scale(0.3, 0.65, 0.3)
rightLowerArm:rotate('z', 10)
rightLowerArm:translate(0.1, -0.5, 0)
rightLowerArm:set_material(white)



node8 = gr.node('node8')
joint1:add_child(node8)
--node8:scale( 0.1, 0.1, 0.1 )
node8:translate(0.0, 0.8, 0.2)

joint8 = gr.joint('headJoint',{-30, 0, 30}, {-45, 0, 45})
node8:add_child(joint8)

head = gr.mesh('sphere', 'head')
joint8:add_child(head)
--head:scale(1/0.6, 1/0.7, 1/0.6);
head:scale(0.3, 0.2, 0.25)
--head:translate(0.0, 0.05, 0.0)
head:set_material(white)


leftEye = gr.mesh('sphere', 'leftEye')
head:add_child(leftEye)
leftEye:scale(1/0.3,1/0.2,1/0.25)
leftEye:scale(0.05,0.05,0.05)
leftEye:translate(-0.4, 0.1, 0.75)

rightEye = gr.mesh('sphere', 'rightEye')
head:add_child(rightEye)
rightEye:scale(1/0.3,1/0.2,1/0.25)
rightEye:scale(0.05,0.05,0.05)
rightEye:translate(0.4, 0.1, 0.75)

nose = gr.mesh('cube', 'nose')
head:add_child(nose)
nose:scale(1/0.3,1/0.2,1/0.25)
nose:scale(0.25, 0.01, 0.03)
nose:translate(0,0.1,0.935)


fnode1 = gr.node('fnode1')
joint6:add_child(fnode1)
--fnode1:scale( 0.05, 0.05, 0.05 )
fnode1:rotate('y',-90)
fnode1:translate(-0.25, -1.05, -0.03)

fjoint1 = gr.joint('fjoint1',{0, 0, 60}, {0, 0, 0})
fnode1:add_child(fjoint1)

finger1 = gr.mesh('sphere', 'finger1')
fjoint1:add_child(finger1)
finger1:scale( 0.05, 0.10, 0.05)
finger1:translate(0,-0.1,0)
finger1:set_material(white)


fnode2 = gr.node('fnode2')
joint6:add_child(fnode2)
--fnode1:scale( 0.05, 0.05, 0.05 )
fnode2:rotate('y',-90)
fnode2:translate(-0.25, -1.04, 0.02)

fjoint2 = gr.joint('fjoint2',{0, 0, 60}, {0, 0, 0})
fnode2:add_child(fjoint2)

finger2 = gr.mesh('sphere', 'finger2')
fjoint2:add_child(finger2)
finger2:scale( 0.05, 0.10, 0.05)
finger2:translate(0,-0.1,0)
finger2:set_material(white)



fnode3 = gr.node('fnode3')
joint6:add_child(fnode3)
--fnode1:scale( 0.05, 0.05, 0.05 )
fnode3:rotate('y',-90)
fnode3:translate(-0.23, -1.02, 0.07)

fjoint3 = gr.joint('fjoint3',{0, 0, 60}, {0, 0, 0})
fnode3:add_child(fjoint3)

finger3 = gr.mesh('sphere', 'finger3')
fjoint3:add_child(finger3)
finger3:scale( 0.05, 0.10, 0.05)
finger3:translate(0,-0.1,0)
finger3:set_material(white)



fnode4 = gr.node('fnode4')
joint6:add_child(fnode4)
fnode4:rotate('y',-90)
--fnode1:scale( 0.05, 0.05, 0.05 )
fnode4:translate(-0.2, -1.01, 0.12)

fjoint4 = gr.joint('fjoint4',{0, 0, 60}, {0, 0, 0})
fnode4:add_child(fjoint4)

finger4 = gr.mesh('sphere', 'finger4')
fjoint4:add_child(finger4)
finger4:scale( 0.05, 0.10, 0.05)
finger4:translate(0,-0.1,0)
finger4:set_material(white)



fnode5 = gr.node('fnode5')
joint7:add_child(fnode5)
fnode5:rotate('y', 90)
--fnode1:scale( 0.05, 0.05, 0.05 )
fnode5:translate(0.25, -1.05, -0.03)

fjoint5 = gr.joint('fjoint5',{0, 0, 60}, {0, 0, 0})
fnode5:add_child(fjoint5)

finger5 = gr.mesh('sphere', 'finger5')
fjoint5:add_child(finger5)
finger5:scale( 0.05, 0.10, 0.05)
finger5:translate(0,-0.1,0)
finger5:set_material(white)


fnode6 = gr.node('fnode6')
joint7:add_child(fnode6)
fnode6:rotate('y', 90)
--fnode1:scale( 0.05, 0.05, 0.05 )
fnode6:translate(0.25, -1.04, 0.02)

fjoint6 = gr.joint('fjoint6',{0, 0, 60}, {0, 0, 0})
fnode6:add_child(fjoint6)

finger6 = gr.mesh('sphere', 'finger6')
fjoint6:add_child(finger6)
finger6:scale( 0.05, 0.10, 0.05)
finger6:translate(0,-0.1,0)
finger6:set_material(white)



fnode7 = gr.node('fnode7')
joint7:add_child(fnode7)
fnode7:rotate('y', 90)
--fnode1:scale( 0.05, 0.05, 0.05 )
fnode7:translate(0.23, -1.02, 0.07)

fjoint7 = gr.joint('fjoint7',{0, 0, 60}, {0, 0, 0})
fnode7:add_child(fjoint7)

finger7 = gr.mesh('sphere', 'finger7')
fjoint7:add_child(finger7)
finger7:scale( 0.05, 0.10, 0.05)
finger7:translate(0,-0.1,0)
finger7:set_material(white)



fnode8 = gr.node('fnode8')
joint7:add_child(fnode8)
fnode8:rotate('y', 90)
--fnode1:scale( 0.05, 0.05, 0.05 )
fnode8:translate(0.2, -1.01, 0.12)

fjoint8 = gr.joint('fjoint8',{0, 0, 60}, {0, 0, 0})
fnode8:add_child(fjoint8)

finger8 = gr.mesh('sphere', 'finger8')
fjoint8:add_child(finger8)
finger8:scale( 0.05, 0.10, 0.05)
finger8:translate(0,-0.1,0)
finger8:set_material(white)



return rootnode
