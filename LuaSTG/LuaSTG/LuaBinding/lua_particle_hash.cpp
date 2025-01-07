// Write by Kuanlan (Kuanlan@outlook.com)
// Minimum perfect hash function for Lua language binding.
// This source file is generated by code generator.
// Don't modify this file directly, please modify the generator configuration.

#include "lua_particle_hash.hpp"

LuaSTGPlus::Particle2DMember LuaSTGPlus::MapParticle2DMember(const char* key) {
  switch(key[0]) {
    case 'a':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::Particle2DMember::a;
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle2DMember::ax;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle2DMember::ay;
          }
          break;
      }
      break;
    case 'b':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::Particle2DMember::b;
      }
      break;
    case 'c':
      switch(key[1]) {
        case 'o':
          switch(key[2]) {
            case 'l':
              switch(key[3]) {
                case 'o':
                  switch(key[4]) {
                    case 'r':
                      switch(key[5]) {
                        case '\0':
                          return LuaSTGPlus::Particle2DMember::color;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'e':
      switch(key[1]) {
        case 'x':
          switch(key[2]) {
            case 't':
              switch(key[3]) {
                case 'r':
                  switch(key[4]) {
                    case 'a':
                      switch(key[5]) {
                        case '1':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::Particle2DMember::extra1;
                          }
                          break;
                        case '2':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::Particle2DMember::extra2;
                          }
                          break;
                        case '3':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::Particle2DMember::extra3;
                          }
                          break;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'g':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::Particle2DMember::g;
      }
      break;
    case 'o':
      switch(key[1]) {
        case 'm':
          switch(key[2]) {
            case 'i':
              switch(key[3]) {
                case 'g':
                  switch(key[4]) {
                    case 'a':
                      switch(key[5]) {
                        case '\0':
                          return LuaSTGPlus::Particle2DMember::omiga;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'r':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::Particle2DMember::r;
        case 'o':
          switch(key[2]) {
            case 't':
              switch(key[3]) {
                case '\0':
                  return LuaSTGPlus::Particle2DMember::rot;
              }
              break;
          }
          break;
      }
      break;
    case 's':
      switch(key[1]) {
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle2DMember::sx;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle2DMember::sy;
          }
          break;
      }
      break;
    case 't':
      switch(key[1]) {
        case 'i':
          switch(key[2]) {
            case 'm':
              switch(key[3]) {
                case 'e':
                  switch(key[4]) {
                    case 'r':
                      switch(key[5]) {
                        case '\0':
                          return LuaSTGPlus::Particle2DMember::timer;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'v':
      switch(key[1]) {
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle2DMember::vx;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle2DMember::vy;
          }
          break;
      }
      break;
    case 'x':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::Particle2DMember::x;
      }
      break;
    case 'y':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::Particle2DMember::y;
      }
      break;
  }
  return LuaSTGPlus::Particle2DMember::__NOT_FOUND;
}

LuaSTGPlus::Particle3DMember LuaSTGPlus::MapParticle3DMember(const char* key) {
  switch(key[0]) {
    case 'a':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::Particle3DMember::a;
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle3DMember::ax;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle3DMember::ay;
          }
          break;
      }
      break;
    case 'b':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::Particle3DMember::b;
      }
      break;
    case 'c':
      switch(key[1]) {
        case 'o':
          switch(key[2]) {
            case 'l':
              switch(key[3]) {
                case 'o':
                  switch(key[4]) {
                    case 'r':
                      switch(key[5]) {
                        case '\0':
                          return LuaSTGPlus::Particle3DMember::color;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'e':
      switch(key[1]) {
        case 'x':
          switch(key[2]) {
            case 't':
              switch(key[3]) {
                case 'r':
                  switch(key[4]) {
                    case 'a':
                      switch(key[5]) {
                        case '1':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::Particle3DMember::extra1;
                          }
                          break;
                        case '2':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::Particle3DMember::extra2;
                          }
                          break;
                        case '3':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::Particle3DMember::extra3;
                          }
                          break;
                        case '4':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::Particle3DMember::extra4;
                          }
                          break;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'g':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::Particle3DMember::g;
      }
      break;
    case 'o':
      switch(key[1]) {
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle3DMember::ox;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle3DMember::oy;
          }
          break;
      }
      break;
    case 'r':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::Particle3DMember::r;
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle3DMember::rx;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle3DMember::rz;
          }
          break;
      }
      break;
    case 's':
      switch(key[1]) {
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle3DMember::sx;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle3DMember::sy;
          }
          break;
      }
      break;
    case 't':
      switch(key[1]) {
        case 'i':
          switch(key[2]) {
            case 'm':
              switch(key[3]) {
                case 'e':
                  switch(key[4]) {
                    case 'r':
                      switch(key[5]) {
                        case '\0':
                          return LuaSTGPlus::Particle3DMember::timer;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'v':
      switch(key[1]) {
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle3DMember::vx;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::Particle3DMember::vz;
          }
          break;
      }
      break;
    case 'x':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::Particle3DMember::x;
      }
      break;
    case 'y':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::Particle3DMember::z;
      }
      break;
  }
  return LuaSTGPlus::Particle3DMember::__NOT_FOUND;
}

LuaSTGPlus::TexParticle2DMember LuaSTGPlus::MapTexParticle2DMember(const char* key) {
  switch(key[0]) {
    case 'a':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle2DMember::a;
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle2DMember::ax;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle2DMember::ay;
          }
          break;
      }
      break;
    case 'b':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle2DMember::b;
      }
      break;
    case 'c':
      switch(key[1]) {
        case 'o':
          switch(key[2]) {
            case 'l':
              switch(key[3]) {
                case 'o':
                  switch(key[4]) {
                    case 'r':
                      switch(key[5]) {
                        case '\0':
                          return LuaSTGPlus::TexParticle2DMember::color;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'e':
      switch(key[1]) {
        case 'x':
          switch(key[2]) {
            case 't':
              switch(key[3]) {
                case 'r':
                  switch(key[4]) {
                    case 'a':
                      switch(key[5]) {
                        case '1':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::TexParticle2DMember::extra1;
                          }
                          break;
                        case '2':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::TexParticle2DMember::extra2;
                          }
                          break;
                        case '3':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::TexParticle2DMember::extra3;
                          }
                          break;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'g':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle2DMember::g;
      }
      break;
    case 'h':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle2DMember::h;
      }
      break;
    case 'o':
      switch(key[1]) {
        case 'm':
          switch(key[2]) {
            case 'i':
              switch(key[3]) {
                case 'g':
                  switch(key[4]) {
                    case 'a':
                      switch(key[5]) {
                        case '\0':
                          return LuaSTGPlus::TexParticle2DMember::omiga;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'r':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle2DMember::r;
        case 'o':
          switch(key[2]) {
            case 't':
              switch(key[3]) {
                case '\0':
                  return LuaSTGPlus::TexParticle2DMember::rot;
              }
              break;
          }
          break;
      }
      break;
    case 's':
      switch(key[1]) {
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle2DMember::sx;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle2DMember::sy;
          }
          break;
      }
      break;
    case 't':
      switch(key[1]) {
        case 'i':
          switch(key[2]) {
            case 'm':
              switch(key[3]) {
                case 'e':
                  switch(key[4]) {
                    case 'r':
                      switch(key[5]) {
                        case '\0':
                          return LuaSTGPlus::TexParticle2DMember::timer;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'u':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle2DMember::u;
      }
      break;
    case 'v':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle2DMember::v;
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle2DMember::vx;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle2DMember::vy;
          }
          break;
      }
      break;
    case 'w':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle2DMember::w;
      }
      break;
    case 'x':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle2DMember::x;
      }
      break;
    case 'y':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle2DMember::y;
      }
      break;
  }
  return LuaSTGPlus::TexParticle2DMember::__NOT_FOUND;
}

LuaSTGPlus::TexParticle3DMember LuaSTGPlus::MapTexParticle3DMember(const char* key) {
  switch(key[0]) {
    case 'a':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle3DMember::a;
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle3DMember::ax;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle3DMember::az;
          }
          break;
      }
      break;
    case 'b':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle3DMember::b;
      }
      break;
    case 'c':
      switch(key[1]) {
        case 'o':
          switch(key[2]) {
            case 'l':
              switch(key[3]) {
                case 'o':
                  switch(key[4]) {
                    case 'r':
                      switch(key[5]) {
                        case '\0':
                          return LuaSTGPlus::TexParticle3DMember::color;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'e':
      switch(key[1]) {
        case 'x':
          switch(key[2]) {
            case 't':
              switch(key[3]) {
                case 'r':
                  switch(key[4]) {
                    case 'a':
                      switch(key[5]) {
                        case '1':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::TexParticle3DMember::extra1;
                          }
                          break;
                        case '2':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::TexParticle3DMember::extra2;
                          }
                          break;
                        case '3':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::TexParticle3DMember::extra3;
                          }
                          break;
                        case '4':
                          switch(key[6]) {
                            case '\0':
                              return LuaSTGPlus::TexParticle3DMember::extra4;
                          }
                          break;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'g':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle3DMember::g;
      }
      break;
    case 'h':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle3DMember::h;
      }
      break;
    case 'o':
      switch(key[1]) {
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle3DMember::ox;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle3DMember::oz;
          }
          break;
      }
      break;
    case 'r':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle3DMember::r;
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle3DMember::rx;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle3DMember::ry;
          }
          break;
      }
      break;
    case 's':
      switch(key[1]) {
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle3DMember::sx;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle3DMember::sy;
          }
          break;
      }
      break;
    case 't':
      switch(key[1]) {
        case 'i':
          switch(key[2]) {
            case 'm':
              switch(key[3]) {
                case 'e':
                  switch(key[4]) {
                    case 'r':
                      switch(key[5]) {
                        case '\0':
                          return LuaSTGPlus::TexParticle3DMember::timer;
                      }
                      break;
                  }
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 'u':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle3DMember::u;
      }
      break;
    case 'v':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle3DMember::v;
        case 'x':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle3DMember::vx;
          }
          break;
        case 'y':
          switch(key[2]) {
            case '\0':
              return LuaSTGPlus::TexParticle3DMember::vz;
          }
          break;
      }
      break;
    case 'w':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle3DMember::w;
      }
      break;
    case 'x':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle3DMember::x;
      }
      break;
    case 'y':
      switch(key[1]) {
        case '\0':
          return LuaSTGPlus::TexParticle3DMember::y;
      }
      break;
  }
  return LuaSTGPlus::TexParticle3DMember::__NOT_FOUND;
}
