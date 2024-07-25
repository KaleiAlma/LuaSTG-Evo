local test = require("test")

---@class test.Module.TextInput : test.Base
local M = {}

local KEY = lstg.Input.Keyboard

local left = false
local left_pre = false
local left_press = false
local right = false
local right_pre = false
local right_press = false
local v = false
local v_pre = false
local v_press = false

function M:onCreate()
    local old_pool = lstg.GetResourceStatus()
    lstg.SetResourceStatus("global")
    lstg.LoadTexture('tex:white', 'res/white.png')
    lstg.LoadImage('img:white', 'tex:white', 0, 0, 16, 16)
    lstg.SetImageState('img:white', '', lstg.Color(0xBB000000))
    lstg.LoadTTF("ttf:test1", "res/msyh.ttc", 0, 26)
    lstg.SetResourceStatus(old_pool)
    lstg.Window.clearTextInput()
    lstg.Window.setTextInputEnable(true)
    lstg.FontRenderer.SetFontProvider('ttf:test1')
end

function M:onDestroy()
    lstg.RemoveResource("global", 8, "ttf:test1")
    lstg.RemoveResource("global", 2, "img:white")
    lstg.RemoveResource("global", 1, "tex:white")
    lstg.Window.setTextInputEnable(false)
    lstg.Window.clearTextInput()
end

function M:onUpdate()
    left_pre = left
    left = KEY.GetKeyState(KEY.Left)
    left_press = left and not left_pre
    right_pre = right
    right = KEY.GetKeyState(KEY.Right)
    right_press = right and not right_pre
    v_pre = v
    v = KEY.GetKeyState(KEY.V)
    v_press = v and not v_pre

    local move = 0

    if left_press then
        move = move - 1
    end
    if right_press then
        move = move + 1
    end

    local pos = math.min(math.max(lstg.Window.getTextCursorPos() + move, 0), lstg.Window.getTextInputLength())

    lstg.Window.setTextCursorPos(pos)

    if
        v_press and
        (KEY.GetKeyState(KEY.LeftControl) or KEY.GetKeyState(KEY.RightControl))
    then
        local text, _ = string.gsub(lstg.Window.getClipboardText(), '\n', ' ')
        text = string.gsub(text, '\r', ' ')
        lstg.Window.insertInputTextAtCursor(text, true)
    end
end

function M:onRender()
    window:applyCameraV()
    local x, y = window.width / 2, window.height / 2

    local rawc = lstg.Window.getTextCursorPosRaw()
    local text = lstg.Window.getTextInput()
    local ime = lstg.Window.getIMEComp()
    local rdr_text = string.sub(text, 1, rawc) .. ime .. string.sub(text, rawc + 1)
    local imec = math.max(lstg.Window.getIMECursorPos(), 0)

    local l, r, b, t = lstg.FontRenderer.MeasureTextBoundary(rdr_text)
    local cl, cr, cb, ct = lstg.FontRenderer.MeasureTextBoundary(string.sub(rdr_text, 1, rawc))
    local c2l, c2r, c2b, c2t = lstg.FontRenderer.MeasureTextBoundary(string.sub(rdr_text, 1, rawc + imec))
    local il, ir, ib, it = lstg.FontRenderer.MeasureTextBoundary(ime)
    local w, h = r - l, t - b
    local cw, ch = cr - cl, ct - cb
    local c2w, c2h = c2r - c2l, c2t - c2b
    local iw, ih = ir - il, it - ib
    local lh = lstg.FontRenderer.GetFontLineHeight()

    lstg.RenderTTF("ttf:test1", rdr_text, x - 64, x + 64, y, y, 1 + 4, lstg.Color(255, 255, 255, 255), 2)
    lstg.Render('img:white', c2r - r * 0.5 + x, y, 0, 0.1, 1.8)
    lstg.RenderRect('img:white', cr - r * 0.5 + x, cr - r * 0.5 + x + iw, y - lh * 0.5, y - lh * 0.5 - 3)
end

test.registerTest("test.Module.TextInput", M)
