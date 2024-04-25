local test = require("test")

---@class test.Module.RenderTarget : test.Base
local M = {}

local function load_texture(f)
    lstg.LoadTexture("tex:" .. f, "res/" .. f, false)
    local w, h = lstg.GetTextureSize("tex:" .. f)
    lstg.LoadImage("img:" .. f, "tex:" .. f, 0, 0, w, h)
end

local function unload_texture(f)
    lstg.RemoveResource("global", 2, "img:" .. f)
    lstg.RemoveResource("global", 1, "tex:" .. f)
end

function M:onCreate()
    local old_pool = lstg.GetResourceStatus()
    lstg.SetResourceStatus("global")
    lstg.CreateRenderTarget("rt:test1")
    lstg.CreateRenderTarget("rt:test2")
    load_texture("block.png")
    lstg.SetResourceStatus(old_pool)
    self.press_key = false
end

function M:onDestroy()
    lstg.RemoveResource("global", 1, "rt:test1")
    lstg.RemoveResource("global", 1, "rt:test2")
    unload_texture("block.png")
end

function M:onUpdate()
    local Keyboard = lstg.Input.Keyboard
    if not self.press_key then
        if Keyboard.GetKeyState(Keyboard.D1) then
            window:setSize(1280, 720)
            self.press_key = true
        elseif Keyboard.GetKeyState(Keyboard.D2) then
            window:setSize(1920, 1080)
            self.press_key = true
        end
    else
        if not Keyboard.GetKeyState(Keyboard.D1) and not Keyboard.GetKeyState(Keyboard.D2) then
            self.press_key = false
        end
    end
end

function M:onRender()
    lstg.PushRenderTarget('rt:test1');
    window:applyCameraV()
    local scale = 0.5
    -- lstg.Render("img:sRGB.png", window.width / 4 * 1, window.height / 2, 0, scale)
    -- lstg.Render("img:linear.png", window.width / 4 * 3, window.height / 2, 0, scale)
    scale = 0.5
    lstg.Render("img:block.png", window.width / 4 * 2, window.height / 4 * 1, 0, scale)
    -- lstg.Render("img:block.qoi", window.width / 4 * 2, window.height / 4 * 3, 0, scale)
    lstg.PopRenderTarget();
end

test.registerTest("test.Module.RenderTarget", M)
