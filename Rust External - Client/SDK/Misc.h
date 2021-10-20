#pragma once

void Misc(UINT64 TodCycle)
{
	if (Value::bools::Visuals::World::AlwaysDay)
		safe_write(TodCycle + 0x10, Value::floats::Visuals::World::Time, float);

	if (Value::bools::Player::PlayerWalk::Spiderman)
		LocalPlayer.BasePlayer->SpiderMan();

	if (Value::bools::Player::PlayerWalk::FakeAdmin)
		LocalPlayer.BasePlayer->FakeAdmin();

	if (Value::bools::Player::PlayerWalk::ChangeGravity)
		LocalPlayer.BasePlayer->GravituMod();

	if (Value::bools::Player::PlayerWalk::InfinityJump)
		LocalPlayer.BasePlayer->InfinityJump();

	if (Value::bools::Player::CustomFov)
		LocalPlayer.BasePlayer->SetFov();
}