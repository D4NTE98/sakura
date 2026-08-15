local enabled = true
local yawMode = 0
local pitchMode = 1
local jitterRange = 42
local fakeOffset = 92
local spinSpeed = 145
local chokePackets = true
local safeOnAttack = true
local sendSide = false
local jitterSide = false
local randomYaw = 0
local nextRandomUpdate = 0

local function normalizeYaw(yaw)
    while yaw > 180 do
        yaw = yaw - 360
    end

    while yaw < -180 do
        yaw = yaw + 360
    end

    return yaw
end

local function isPressed(cmd, flag)
    return LocalPlayer.CheckButton(cmd, flag)
end

local function shouldSkip(cmd, active)
    if not enabled or active == 0 then
        return true
    end

    if not LocalPlayer.IsAlive() then
        return true
    end

    if isPressed(cmd, IN_USE) then
        return true
    end

    if safeOnAttack and (isPressed(cmd, IN_ATTACK) or isPressed(cmd, IN_ATTACK2)) then
        return true
    end

    return false
end

local function applyPitch(angles)
    if pitchMode == 1 then
        angles.x = 89.0
    elseif pitchMode == 2 then
        angles.x = -89.0
    elseif pitchMode == 3 then
        angles.x = jitterSide and 89.0 or -89.0
    elseif pitchMode == 4 then
        angles.x = jitterSide and 179.0 or -179.0
    end
end

local function applyYaw(angles)
    local baseYaw = angles.y
    local now = Game.GetTime()

    if yawMode == 0 then
        local jitter = jitterSide and jitterRange or -jitterRange
        angles.y = normalizeYaw(baseYaw + 180.0 + jitter)
    elseif yawMode == 1 then
        local offset = sendSide and fakeOffset or -fakeOffset
        angles.y = normalizeYaw(baseYaw + 180.0 + offset)
    elseif yawMode == 2 then
        angles.y = normalizeYaw((Game.GetClientTime() * spinSpeed) % 360.0)
    elseif yawMode == 3 then
        if now >= nextRandomUpdate then
            randomYaw = math.random(-180, 180)
            nextRandomUpdate = now + 85
        end

        angles.y = normalizeYaw(baseYaw + randomYaw)
    elseif yawMode == 4 then
        local offset = jitterSide and 90.0 or -90.0
        angles.y = normalizeYaw(baseYaw + offset)
    end
end

local function onMove(frametime, cmd, active)
    if shouldSkip(cmd, active) then
        return
    end

    LocalPlayer.FixMoveStart(cmd)

    local angles = cmd.viewangles

    applyYaw(angles)
    applyPitch(angles)

    cmd.viewangles = angles

    LocalPlayer.FixMoveEnd(cmd)

    if chokePackets then
        sendSide = not sendSide
        Game.SendPacket(sendSide)
    end

    jitterSide = not jitterSide
end

local function onMenu()
    ImGui.Text("Better Anti-Aim")
    ImGui.Text("Standalone Lua anti-aim for Sakura v2.0.2")
    ImGui.Spacing()

    enabled = ImGui.Checkbox("Enabled", enabled)
    safeOnAttack = ImGui.Checkbox("Disable while attacking", safeOnAttack)
    chokePackets = ImGui.Checkbox("Alternate send packets", chokePackets)

    ImGui.Spacing()

    yawMode = ImGui.Combo("Yaw mode", yawMode, "Adaptive jitter\0Fake switch\0Spin\0Random\0Sideways\0")
    pitchMode = ImGui.Combo("Pitch mode", pitchMode, "Off\0Down\0Up\0Jitter\0Fake jitter\0")

    if yawMode == 0 then
        jitterRange = ImGui.Slider("Jitter range", jitterRange, 5, 90)
    elseif yawMode == 1 then
        fakeOffset = ImGui.Slider("Fake offset", fakeOffset, 30, 180)
    elseif yawMode == 2 then
        spinSpeed = ImGui.Slider("Spin speed", spinSpeed, 30, 720)
    end

    ImGui.Spacing()
    ImGui.Text("Disable native Anti-Aim and Fake Lag while this script is active.")
end

math.randomseed(Game.GetTime())
Log.File("betterAA.lua: initializing")
Hooks.Register(SAKURA_CLIENT_MOVE, onMove)
Hooks.Register(SAKURA_MENU_RENDER, onMenu)
Notify.Create("betterAA.lua loaded", 3)
Log.File("betterAA.lua: loaded")
