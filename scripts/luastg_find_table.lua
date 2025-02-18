
local findtable = require("findtable")
local E = findtable.makeEntry

---@type findtable.config
local lstg_GameObject = {
    cpp_namespace = { "LuaSTG" },
    func_name = "MapGameObjectMember",
    enum_name = "GameObjectMember",
    enum_entry = {
        -- basic
        E("status", "STATUS"),
        E("class" , "CLASS" ),
        -- user
        E("timer" , "TIMER" ),
        -- transform
        E("x"     , "X"     ),
        E("y"     , "Y"     ),
        E("rot"   , "ROT"   ),
        E("hscale", "HSCALE"),
        E("vscale", "VSCALE"),
        -- movement
        E("dx"    , "DX"    ),
        E("dy"    , "DY"    ),
        E("omiga" , "OMEGA" ),
        E("ax"    , "AX"    ),
        E("ay"    , "AY"    ),
        E("ag"    , "AG"    ),
        E("vx"    , "VX"    ),
        E("vy"    , "VY"    ),
        E("maxv"  , "MAXV"  ),
        E("maxvx" , "MAXVX" ),
        E("maxvy" , "MAXVY" ),
        E("navi"  , "NAVI"  ),
        -- render
        E("layer" , "LAYER" ),
        E("hide"  , "HIDE"  ),
        E("img"   , "IMG"   ),
        E("rc"    , "RES_RC"),
        E("ani"   , "ANI"   ),
        E("_blend", "_BLEND"),
        E("_color", "_COLOR"),
        E("_a"    , "_A"    ),
        E("_r"    , "_R"    ),
        E("_g"    , "_G"    ),
        E("_b"    , "_B"    ),
        -- collision
        E("bound" , "BOUND" ),
        E("group" , "GROUP" ),
        E("colli" , "COLLI" ),
        E("a"     , "A"     ),
        E("b"     , "B"     ),
        E("rect"  , "RECT"  ),
        E("collider", "COLLIDER"), -- TODO: remove it
        -- TODO: fuck ex+
        E("_angle", "VANGLE"  ),
        E("_speed", "VSPEED"  ),
        E("_pos", "VPOS"  ),
        E("_vel", "VVEL"  ),
        E("_accel", "VACCEL"  ),
        E("_scale", "VVSCALE"  ),
        E("pause" , "PAUSE"  ),
        E("nopause", "IGNORESUPERPAUSE"  ),
        E("rmove" , "RESOLVEMOVE"  ),
        E("world" , "WORLD"  ),
    },
}

---@type findtable.config
local lstg_BlendMode = {
    cpp_namespace = { "LuaSTG" },
    func_name = "MapBlendModeX",
    enum_name = "BlendModeX",
    enum_entry = {
        -- plus
        E("mul+alpha" , "MulAlpha"  ,  1),
        E("mul+add"   , "MulAdd"    ,  2),
        E("mul+rev"   , "MulRev"    ,  3),
        E("mul+sub"   , "MulSub"    ,  4),
        E("add+alpha" , "AddAlpha"  ,  5),
        E("add+add"   , "AddAdd"    ,  6),
        E("add+rev"   , "AddRev"    ,  7),
        E("add+sub"   , "AddSub"    ,  8),
        -- ex plus
        E("alpha+bal" , "AlphaBal"  ,  9),
        -- sub
        E("mul+min"   , "MulMin"    , 10),
        E("mul+max"   , "MulMax"    , 11),
        E("mul+mul"   , "MulMutiply", 12),
        E("mul+screen", "MulScreen" , 13),
        E("add+min"   , "AddMin"    , 14),
        E("add+max"   , "AddMax"    , 15),
        E("add+mul"   , "AddMutiply", 16),
        E("add+screen", "AddScreen" , 17),
        E("one"       , "One"       , 18),
        -- faggot
        E("hue+alpha" , "HueAlpha"  , 19),
        E("hue+add"   , "HueAdd"    , 20),
        E("hue+rev"   , "HueRev"    , 21),
        E("hue+sub"   , "HueSub"    , 22),
        E("hue+min"   , "HueMin"    , 23),
        E("hue+max"   , "HueMax"    , 24),
        E("hue+mul"   , "HueMul"    , 25),
        E("hue+screen", "HueScreen" , 26),
    },
}

---@type findtable.config
local lstg_Color = {
    cpp_namespace = { "LuaSTG" },
    func_name = "MapColorMember",
    enum_name = "ColorMember",
    enum_entry = {
        -- basic
        E("a"   , "m_a"   ),
        E("r"   , "m_r"   ),
        E("g"   , "m_g"   ),
        E("b"   , "m_b"   ),
        E("argb", "m_argb"),
        E("ARGB", "f_ARGB"),
        -- ext
        E("h"   , "m_h"   ),
        E("s"   , "m_s"   ),
        E("v"   , "m_v"   ),
        E("AHSV", "f_AHSV"),
    },
}

---@type findtable.config
local lstg_Rect = {
    cpp_namespace = { "LuaSTG" },
    func_name = "MapRectMember",
    enum_name = "RectMember",
    enum_entry = {
        -- basic
        E("l"         , "m_l"         ),
        E("t"         , "m_t"         ),
        E("r"         , "m_r"         ),
        E("b"         , "m_b"         ),
        E("lt"        , "m_lt"        ),
        E("rt"        , "m_rt"        ),
        E("rb"        , "m_rb"        ),
        E("lb"        , "m_lb"        ),
        E("ratio"     , "m_ratio"     ),
        E("width"     , "m_width"     ),
        E("height"    , "m_height"    ),
        E("center"    , "m_center"    ),
        E("dimension" , "m_dimension" ),
        E("Resize"    , "f_Resize"    ),
        E("GetPointInside","f_GetPointInside"),
        E("IsPointInside","f_IsPointInside"),
    },
}
---@type findtable.config
local lstg_Vector2 = {
    cpp_namespace = { "LuaSTG" },
    func_name = "MapVector2Member",
    enum_name = "Vector2Member",
    enum_entry = {
        -- basic
        E("x"         , "m_x"         ),
        E("y"         , "m_y"         ),
        E("Dot"       , "f_Dot"       ),
        E("Length"    , "f_Length"    ),
        E("Normalize" , "f_Normalize" ),
        E("Normalized", "f_Normalized"),
        E("Angle"     , "f_Angle"     ),
    },
}

---@type findtable.config
local lstg_Vector3 = {
    cpp_namespace = { "LuaSTG" },
    func_name = "MapVector3Member",
    enum_name = "Vector3Member",
    enum_entry = {
        -- basic
        E("x"         , "m_x"         ),
        E("y"         , "m_y"         ),
        E("z"         , "m_z"         ),
        E("Dot"       , "f_Dot"       ),
        E("Length"    , "f_Length"    ),
        E("Normalize" , "f_Normalize" ),
        E("Normalized", "f_Normalized"),
    },
}

---@type findtable.config
local lstg_Vector4 = {
    cpp_namespace = { "LuaSTG" },
    func_name = "MapVector4Member",
    enum_name = "Vector4Member",
    enum_entry = {
        -- basic
        E("x"         , "m_x"         ),
        E("y"         , "m_y"         ),
        E("z"         , "m_z"         ),
        E("w"         , "m_w"         ),
        E("Dot"       , "f_Dot"       ),
        E("Length"    , "f_Length"    ),
        E("Normalize" , "f_Normalize" ),
        E("Normalized", "f_Normalized"),
    },
}

---@type findtable.config
local lstg_Matrix2 = {
    cpp_namespace = { "LuaSTG" },
    func_name = "MapMatrix2Member",
    enum_name = "Matrix2Member",
    enum_entry = {
        -- basic
        E("Determinant", "f_Determinant"),
        E("Inverse"    , "f_Inverse"    ),
        E("Transpose"  , "f_Transpose"  ),
    },
}

---@type findtable.config
local lstg_Matrix3 = {
    cpp_namespace = { "LuaSTG" },
    func_name = "MapMatrix3Member",
    enum_name = "Matrix3Member",
    enum_entry = {
        -- basic
        E("Determinant", "f_Determinant"),
        E("Inverse"    , "f_Inverse"    ),
        E("Transpose"  , "f_Transpose"  ),
    },
}

---@type findtable.config
local lstg_Matrix4 = {
    cpp_namespace = { "LuaSTG" },
    func_name = "MapMatrix4Member",
    enum_name = "Matrix4Member",
    enum_entry = {
        -- basic
        E("Determinant", "f_Determinant"),
        E("Inverse"    , "f_Inverse"    ),
        E("Transpose"  , "f_Transpose"  ),
    },
}

findtable.makeSource("lua_luastg_hash", {
    lstg_GameObject,
    lstg_BlendMode,
    lstg_Color,
    lstg_Rect,
    lstg_Vector2,
    lstg_Vector3,
    lstg_Vector4,
    lstg_Matrix2,
    lstg_Matrix3,
    lstg_Matrix4,
})
