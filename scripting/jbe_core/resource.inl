/*
*	Shared Functions Header File
*/

/*==================================================
 #			-> Ресурсы ->
====================================================*/

/* <- Player models <- */
new models_player[PLAYER_MODELS_LIST][MODEL_STRING_SIZE] = { "leet", "leet", "sas", "sas", "vip", "vip", };

/* -> Models -> */
// Hand Pn & Gr
new model_v_hand[MODEL_STRING_SIZE] = "models/v_knife.mdl";
new model_p_hand[MODEL_STRING_SIZE] = "models/p_knife.mdl";
new model_v_baton[MODEL_STRING_SIZE] = "models/v_knife.mdl";
new model_p_baton[MODEL_STRING_SIZE] = "models/p_knife.mdl";

// Box
new model_v_boxing_gloves_red[MODEL_STRING_SIZE] = "models/v_knife.mdl";
new model_p_boxing_gloves_red[MODEL_STRING_SIZE] = "models/p_knife.mdl";
new model_v_boxing_gloves_blue[MODEL_STRING_SIZE] = "models/v_knife.mdl";
new model_p_boxing_gloves_blue[MODEL_STRING_SIZE] = "models/p_knife.mdl";

// Shop
new model_v_balisong[MODEL_STRING_SIZE] = "models/v_knife.mdl";
new model_p_balisong[MODEL_STRING_SIZE] = "models/p_knife.mdl";
new model_v_baseballbat[MODEL_STRING_SIZE] = "models/v_knife.mdl";
new model_p_baseballbat[MODEL_STRING_SIZE] = "models/p_knife.mdl";
new model_v_syringe[MODEL_STRING_SIZE] = "models/v_knife.mdl";

// Soccer
new model_ball[MODEL_STRING_SIZE];
new model_v_hand_ball[MODEL_STRING_SIZE] =	"models/v_knife.mdl";

// Прочее
new model_p_bracelets[MODEL_STRING_SIZE] = "models/p_knife.mdl";
new model_v_bracelets[MODEL_STRING_SIZE] = "models/v_knife.mdl";

/* <- Models <- */

/* -> Sounds -> */
// Hand Pn & Gr
new sound_hand_hit[SOUND_STRING_SIZE] = "weapons/knife_hit1.wav";
new sound_hand_slash[SOUND_STRING_SIZE] = "weapons/knife_slash1.wav";
new sound_hand_deploy[SOUND_STRING_SIZE] = "weapons/knife_deploy1.wav";

new sound_baton_deploy[SOUND_STRING_SIZE] = "jb_engine/weapons/baton_deploy.wav";
new sound_baton_hitwall[SOUND_STRING_SIZE] = "jb_engine/weapons/baton_hitwall.wav";
new sound_baton_slash[SOUND_STRING_SIZE] = "jb_engine/weapons/baton_slash.wav";
new sound_baton_stab[SOUND_STRING_SIZE] = "jb_engine/weapons/baton_stab.wav";
new sound_baton_hit[SOUND_STRING_SIZE] = "jb_engine/weapons/baton_hit.wav";

// Soccer
new sound_bounce_ball[SOUND_STRING_SIZE];
new sound_grab_ball[SOUND_STRING_SIZE];
new sound_kick_ball[SOUND_STRING_SIZE];
new sound_whitle_start[SOUND_STRING_SIZE]; // свисток 1 раз
new sound_whitle_end[SOUND_STRING_SIZE]; // свисток 3 раза

// Box
new sound_gloves_hit[SOUND_STRING_SIZE] = "jb_engine/boxing/gloves_hit.wav";
new sound_super_hit[SOUND_STRING_SIZE] = "jb_engine/boxing/super_hit.wav";
new sound_gong[SOUND_STRING_SIZE];

// Shop

new sound_balisong_deploy[SOUND_STRING_SIZE] = "jb_engine/shop/balisong_deploy.wav";
new sound_balisong_hitwall[SOUND_STRING_SIZE] = "jb_engine/shop/balisong_hitwall.wav";
new sound_balisong_slash[SOUND_STRING_SIZE] = "jb_engine/shop/balisong_slash.wav";
new sound_balisong_hit[SOUND_STRING_SIZE] = "jb_engine/shop/balisong_hit.wav";

new sound_grenade_frost_explosion[SOUND_STRING_SIZE];
new sound_freeze_player[SOUND_STRING_SIZE];
new sound_defrost_player[SOUND_STRING_SIZE];
new sound_syringe_use[SOUND_STRING_SIZE];

new sound_prison_riot[SOUND_STRING_SIZE];
new sound_hook[SOUND_STRING_SIZE];
new sound_duel_ready[SOUND_STRING_SIZE];
new sound_freeday_end[SOUND_STRING_SIZE];
new sounds_countdown[11][SOUND_STRING_SIZE];
/* <- Sounds <- */

/*==================================================
 #			<- Ресурсы <-
====================================================*/