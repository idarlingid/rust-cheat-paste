#pragma once
namespace Value {
	namespace bools {
		namespace Aim {
			bool Enable, VisibleCheck, IgnoreTeam, TargetLine, Fov, Smooth, IgnoreSleepers;
		}
		namespace Player {
			bool CustomFov;
			namespace PlayerWalk {
				bool Spiderman, FakeAdmin, ChangeGravity, InfinityJump, AntiAim;
			}
			namespace PlayerModel {
			}
		}

		namespace Visuals {
			namespace PlayerPanel {
				bool Enable, Name, HP, Weapons;
			}
			namespace ESP {
				bool Enable, Player, NPC, IgnoreSleeper;
				bool Name, Box, Distance, SteamID, Health, Weapon;
			}
			namespace World {
				bool AlwaysDay;
			}
		}
		namespace Misc {

		}
		namespace Weapon {
			bool IsAutimatic, NoSpread, NoRecoil;
		}
	}
	namespace floats {
		namespace Aim {
			int Distance = 400;
			int Fov = 50;
			float Smooth = 5;
		}
		namespace Screen {
			float H;
			float W;
		}
		namespace Player {
			float CustomFov = 90;
			float Gravity = 2;
		}
		namespace Visuals {
			namespace World {
				int Time = 9;
			}
		}
	}
	namespace Colors {
		namespace Aim {
			float Fov[] = { 255, 0, 255, 255 };
			float TargetLine[] = { 255, 0, 255, 255 };
		}
		namespace Visuals {
			namespace ESP {
				float Weapon[] = { 255, 0 ,255, 255 };
				float Box[] = { 255, 0 ,255, 255 };
				float Distance[] = { 255, 0, 255, 255 };
				float Name[] = { 255, 0 ,255, 255 };
				float Health[] = { 255.f , 0.f ,255.f ,255.f };
				float SteamID[] = { 255, 0 ,255, 255 };
			}
		}
	}
}