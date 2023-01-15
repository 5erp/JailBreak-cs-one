// Natives of JailBreak Engine 

#if defined _jbe_natives
	#endinput
#endif
#define _jbe_natives

native jbe_register_day_mode(const szLang[], iBlock, iTime, const szSoundFile[], const szSoundName[], iSoundTime);

native jbe_get_user_team(id);

native jbe_set_user_model(id, const szModel[]);

native jbe_register_game_music(const szTrackName[], const szFileName[]);

native jbe_set_user_rendering(pPlayer, iRenderFx, iRed, iGreen, iBlue, iRenderMode, iRenderAmt);

native jbe_save_user_rendering(pPlayer);