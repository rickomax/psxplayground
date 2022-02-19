function createVector()
	return {0,0,0,0}
end

function createMatrix()
	return {
    1,0,0,
    0,1,0,
    0,0,1,
    0,0,0
	}
end

function getRotationDirection(r, axis)
	return icos(r[axis]), isin(r[axis])
end

function printVector(v)
	return ((v[X]/4096) .. "|" .. (v[Y]/4096) .. "|" .. (v[Z]/4096) .. "|" .. (v[W]/4096))
end

function printMatrix(v)
	return(
	(v[int(1)]) .. "|" .. (v[int(2)]) .. "|" .. (v[int(3)]) .. "\n" ..
	(v[int(4)]) .. "|" .. (v[int(5)]) .. "|" .. (v[int(6)]) .. "\n" ..
	(v[int(7)]) .. "|" .. (v[int(8)]) .. "|" .. (v[int(9)]) .. "\n" ..
	(v[int(10)]) .. "|" .. (v[int(11)]) .. "|" .. (v[int(12)]) .. "\n"
	)
end
--------------------------------------------------------
--GTE Matrices
--------------------------------------------------------
ColorMatrix = {
    1.0,
    0,
    0,
    1.0,
    0,
    0,
    1.0,
    0,
    0,
    0,
    0,
    0
}

LightMatrix = {
    -0.75,
    -0.25,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
}
--------------------------------------------------------
--Physics
--------------------------------------------------------
groundLevel = 0.0
groundPos = createVector()
groundNormal = createVector()
gravity = 0.7
--------------------------------------------------------
--Player
--------------------------------------------------------
playerWalk = {}
playerIdle = {}
playerJump = {}

playerSpeed = 0.0
playerMaxSpeed = 20.0
playerAccel = 2.0
playerFriction = 0.5
playerRotSpeed = 0.01
playerVerticalSpeed = 0.0
playerJumpStrength = 30.0
playerStepHeight = 32

playerYawOffset = 0.25

playerRot = {0.5, playerYawOffset, 0, 0}
playerPos = createVector()
playerDir = createVector()

playerMatrix = createMatrix()

playerColliderHeight = 130
playerColliderScale = 120
playerColliderLocalPos = {0,playerColliderHeight,0,0}
playerColliderPos = createVector()

playerFrame = 1
playerFramesPerUpdate = 0.33

--pike
--playerWalkFrames = 16
--playerIdleFrames = 51
--playerJumpFrames = 4
--playerWalkFormat = "Data/Pyke/PikeWalk_%06d.obj"
--playerIdleFormat = "Data/Pyke/PikeIdle_%06d.obj"
--playerJumpFormat = "Data/Pyke/PikeJump_%06d.obj"
--playerScale = 25
--playerName="Pike"

--frill
playerWalkFrames = 9
playerIdleFrames = 25
playerJumpFrames = 5
playerWalkFormat = "Data/Frill/Frill_Walk_%06d.obj"
playerIdleFormat = "Data/Frill/Frill_Idle_%06d.obj"
playerJumpFormat = "Data/Frill/Frill_Jump_%06d.obj"
playerScale = 3
playerName="Frill"
playerFacePath="Data/FrillFace.bmp"
--------------------------------------------------------
--Camera
--------------------------------------------------------
cameraRot = {0, 0, 0, 0}
cameraPos = createVector()
cameraLocalPos = {0,-200,-700,0}

cameraMatrix = createMatrix()
--------------------------------------------------------
--Map
--------------------------------------------------------
map = {}
mapScale = 512
mapRot = {0.5, 0, 0, 0}
mapPos = createVector()

mapMatrix = createMatrix()
--------------------------------------------------------
--Audios
--------------------------------------------------------
bgAudio = 0
jumpAudio = 0
stepAudio1 = 0
stepAudio2 = 0
--------------------------------------------------------
--Bitmaps
--------------------------------------------------------
playerFace = 0
--------------------------------------------------------
--Misc
--------------------------------------------------------
white = {255,255,255,255}
--lightPos = {0,0,0,0}
--lightData = {255,255,255,100}

function init()
    InitGeom()
    SetGeomOffset(SCREEN_XRES/2, SCREEN_YRES/2)
    SetGeomScreen(SCREEN_XRES/2)
    SetBackColor(128, 128, 128)
    setClearColor(125, 217, 235)
    SetColorMatrix(ColorMatrix)
	bgAudio = loadWAV("Data/Chammel_1.wav", true)
	jumpAudio = loadWAV("Data/jump.wav", false)
	stepAudio1 = loadWAV("Data/step1.wav", false)
	stepAudio2 = loadWAV("Data/step2.wav", false)
	playWAV(bgAudio, 0)
    for v = 1, playerWalkFrames, 1 do
        filename = string.format(playerWalkFormat, int(v - 1))
        objId = loadOBJ(filename, playerScale, 0, 0, 0, 16)
        playerWalk[int(v)] = objId
    end
	for v = 1, playerIdleFrames, 1 do
        filename = string.format(playerIdleFormat, int(v - 1))
        objId = loadOBJ(filename, playerScale, 0, 0, 0, 16)
        playerIdle[int(v)] = objId
    end
	for v = 1, playerJumpFrames, 1 do
        filename = string.format(playerJumpFormat, int(v - 1))
        objId = loadOBJ(filename, playerScale, 0, 0, 0, 16)
        playerJump[int(v)] = objId
    end
	map = loadOBJ("Data/map.obj", mapScale, 0, 0, 0, 16)
	playerFace = loadBMP(playerFacePath, 0, true, false, 16)
end

function checkGroundPos()
	rayOrigin = vectorCopyLV(playerPos)
	rayOrigin[Y] = rayOrigin[Y] - playerStepHeight
	groundLevel, groundPos, groundNormal, groundTriangleIndex = raycast(map, mapMatrix, rayOrigin, {0,1,0,0})
end

function playerGrounded()
	checkGroundPos()
	return playerVerticalSpeed >= 0 and playerPos[Y] >= groundPos[Y] - playerStepHeight
end 

function render()
	lightPos = vectorCopyLV(playerPos)

	cameraRot[Y] = playerRot[Y] - playerYawOffset
	
	cameraPos[X] = playerPos[X]
	cameraPos[Y] = playerPos[Y]
	cameraPos[Z] = playerPos[Z]
	
	cameraInvRot = vectorInvertSV(cameraRot)
	cameraMatrix = RotMatrix(cameraInvRot)
	cameraPos = vectorAddLV(cameraPos, ApplyMatrixLV(cameraMatrix, cameraLocalPos))
	
	cameraFinalMatrix = RotMatrix(cameraRot)
	cameraInvPos = vectorInvertLV(cameraPos)
	cameraViewPos = ApplyMatrixLV(cameraFinalMatrix, cameraInvPos)
	cameraFinalMatrix = TransMatrix(cameraFinalMatrix, cameraViewPos)
	
	if padPressed(1, PAD_LEFT) then
		playerRot[Y] = playerRot[Y] + playerRotSpeed
	elseif padPressed(1, PAD_RIGHT) then
		playerRot[Y] = playerRot[Y] - playerRotSpeed
	end
	
	if padPressed(1, PAD_UP) then
		playerSpeed = playerSpeed + playerAccel
	elseif padPressed(1, PAD_DOWN) then
		playerSpeed = playerSpeed - playerAccel
	end
	
	--if padPressed(1, PAD_SQUARE) then
	--	lightData[W] = lightData[W] + 10
	--elseif padPressed(1, PAD_CIRCLE) then
	--	lightData[W] = lightData[W] - 10
	--end
	
	if playerGrounded() and padDown(1, PAD_CROSS) then
		playerVerticalSpeed = -playerJumpStrength
		playWAV(jumpAudio, 1)
	end
	
	if playerSpeed > 0 then
		playerSpeed = playerSpeed - playerFriction
		if playerSpeed < 0 then
			playerSpeed = 0
		end
	elseif playerSpeed < 0 then
		playerSpeed = playerSpeed + playerFriction
		if playerSpeed > 0 then
			playerSpeed = 0
		end
	end
	if playerSpeed < -playerMaxSpeed then
		playerSpeed = -playerMaxSpeed
	elseif playerSpeed > playerMaxSpeed then
		playerSpeed = playerMaxSpeed
	end
	
	mapMatrix = RotMatrix(mapRot)
    SetLightMatrix(MulMatrix0(LightMatrix, mapMatrix))
	mapMatrix = TransMatrix(mapMatrix, mapPos)
	mapFinalMatrix = CompMatrix(cameraFinalMatrix, mapMatrix)
	drawOBJ(map, mapMatrix, mapFinalMatrix, true, false, white, nil, nil)
	
	--todo
	dirX, dirZ = getRotationDirection(playerRot, Y)
	playerDir = {dirX, 0, dirZ, 0}
	
	if playerGrounded() then
		playerDir = projectOnPlane(playerDir, groundNormal)
	end
	
	playerPos[X] = playerPos[X] + playerDir[X] * playerSpeed
	playerPos[Y] = playerPos[Y] + playerDir[Y] * playerSpeed 
	playerPos[Z] = playerPos[Z] + playerDir[Z] * playerSpeed
	
	if playerGrounded() then
		playerVerticalSpeed = 0
	else
		playerVerticalSpeed = playerVerticalSpeed + gravity
		if playerVerticalSpeed > 50 then
			playerVerticalSpeed = 50
		end
	end
	
	playerPos[Y] = playerPos[Y] + playerVerticalSpeed
	
	playerColliderPos = vectorSubLV(playerPos, playerColliderLocalPos)	
	seperationCollision = sphereOBJIntersection(map, mapMatrix, playerColliderPos, playerColliderScale)
	playerPos = vectorSubLV(playerPos, seperationCollision)

	playerMatrix = RotMatrix(playerRot)
	SetLightMatrix(MulMatrix0(LightMatrix, playerMatrix))
	playerMatrix = TransMatrix(playerMatrix, playerPos)
	playerFinalMatrix = CompMatrix(cameraFinalMatrix, playerMatrix)
	
	if not playerGrounded() then
		drawOBJ(playerJump[int((playerFrame % playerJumpFrames)+1)], playerMatrix, playerFinalMatrix, false, false, white, nil, nil)
	elseif playerSpeed == 0 then	                                                                                      
		drawOBJ(playerIdle[int((playerFrame % playerIdleFrames)+1)], playerMatrix, playerFinalMatrix, false, false, white, nil, nil)
	else                                                                                                                   
		drawOBJ(playerWalk[int((playerFrame % playerWalkFrames)+1)], playerMatrix, playerFinalMatrix, false, false, white, nil, nil)
		curF = playerFrame % (playerWalkFrames)
		if (curF == 0) then
			playWAV(stepAudio1, 2)
		elseif (curF == playerWalkFrames / 2) then
			playWAV(stepAudio2, 3)
		end
	end
	playerFrame = playerFrame + playerFramesPerUpdate
	
	drawGeom()
	FntOpen(64,16,true,playerName)
	drawText()
	drawTile(64,24,128,8,{128,0,0,255})
	drawTile(64,24,iclamp(ilerp(0,128,(-playerVerticalSpeed)/playerJumpStrength),0,128),8,{0,128,0,255})
	drawBMP(playerFace,8,0,{128,128,128,255})
	FntOpen(56,224,true,"Pad to Move. Cross to Jump")
	drawText()
end
