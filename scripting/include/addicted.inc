#if defined _ADDICTINCLUDE_included
 #endinput
#endif
#define _ADDICTINCLUDE_included

#include <colors_csgo>
#include <sdktools>
#include <sdkhooks>
#include <cstrike>

#define SPECMODE_NONE 				0
#define SPECMODE_FIRSTPERSON 		4
#define SPECMODE_3RDPERSON 			5
#define SPECMODE_FREELOOK	 		6

#define HIDEHUD_ALL				 	(1 << 2)
#define HIDEHUD_CSGO_RADAR 			(1 << 12)

#define LoopValidPlayers(%1) for(int %1; %1 < MaxClients; %1++)\
								if(IsValidClient(%1))

#define LoopValidPlayersAndBots(%1) for(int %1; %1 < MaxClients; %1++)\
								if(IsValidClient(%1, false))

#define LoopAlivePlayers(%1) for(int %1; %1 < MaxClients; %1++)\
								if(IsValidClient(%1) && IsPlayerAlive(%1))\
									if (GetClientTeam(%1) != 1)
								
#define LoopAlivePlayersAndBots(%1) for(int %1; %1 < MaxClients; %1++)\
								if(IsValidClient(%1, false) && IsPlayerAlive(%1))								

#define STRING(%1) %1, sizeof(%1)

stock bool IsValidClient(int client, bool noBots=true) {

	if (client < 1 || client > MaxClients)
		return false;
		
	if (!IsClientInGame(client))
		return false;
		
	if (!IsClientConnected(client))
		return false;

	if (noBots)
		if (IsFakeClient(client))
			return false;

	if (IsClientSourceTV(client))
		return false;

	return true;
	
}

stock int GetClientFromSteam(char[] steamid) {

	char client_steamid[32];
	
	LoopValidPlayers(i) {
	
		GetClientAuthId(i, AuthId_Engine, STRING(client_steamid));

		if (StrEqual(steamid, client_steamid))
			return i;
			
	}

	return 0;
	
}

stock int GetClientFromSteam64(int steamid) {

	char client_steamid[32];
	
	LoopValidPlayers(i) {
	
		GetClientAuthId(i, AuthId_SteamID64, STRING(client_steamid));
		char authchar[64];
		IntToString(steamid, authchar, sizeof(authchar));

		if (StrEqual(client_steamid, authchar))
			return i;
			
	}

	return 0;
	
}

stock int GetClientFromSteamUserID(int steamid) {

	char client_steamid[32];
	
	LoopValidPlayers(i) {
	
		GetClientAuthId(i, AuthId_Steam3, STRING(client_steamid));
		char authchar[64];
		IntToString(steamid, authchar, sizeof(authchar));

		ReplaceString(STRING(client_steamid), "[U:1:", "");
		ReplaceString(STRING(client_steamid), "]", "");

		if (StrEqual(client_steamid, authchar))
			return i;
			
	}

	return 0;
	
}

stock int GetClientFromIP(char[] ip) {

	char client_ip[16];
	
	LoopValidPlayers(i) {
	
		GetClientIP(i, STRING(client_steamid));
		
		if (StrEqual(ip, client_ip))
			return i;
			
	}

	return 0;
}

stock bool ClientHasFlag(client, flag) {

	int flags = GetUserFlagBits(client);

	if(flag == 0 || flags & flag || flags & ADMFLAG_ROOT)
		return true;

	return false;
	
}

stock int DoNothingMenuHandler(Handle menu, MenuAction action, int client, int option) {

	if (action == MenuAction_End)
		CloseHandle(menu);
		
}

stock void ConvertSteamID(char[] steamid, int len) {

	ReplaceString(steamid, len, "STEAM_1:", "STEAM_0:", false);
	
}

stock void PrintToChatAndConsole(int client, const char[] szFormat, any ...) {

	char szBuffer[256];
	VFormat(STRING(szBuffer), szFormat, 3);
	
	CPrintToChat(client, szBuffer);
	
	CRemoveTags(STRING(szBuffer));
	PrintToConsole(client, szBuffer);
	
}

stock void PrintToChatAndConsoleAll(const char[] szFormat, any ...) {

	char szBuffer[256];
	VFormat(STRING(szBuffer), szFormat, 3);
	
	LoopValidPlayers(i) {
	
		CPrintToChat(i, szBuffer);
		
		CRemoveTags(STRING(szBuffer));
		PrintToConsole(i, szBuffer);
		
	}
	
}

stock int GetRandomPlayer(int team, int exclude = 1) { 

	int clients[MAXPLAYERS+1];
	int clientCount;

	LoopValidPlayers(i) {
    		
        if (GetClientTeam(i) == team || (team == -1 && GetClientTeam(i) != exclude))
            clients[clientCount++] = i; 
            
    }
   
	return clients[GetRandomInt(0, clientCount - 1)]; 
	
}

stock int GetConnectedPlayerCount(bool bots = false) {

	int num = 0;
	
	if (bots) {
	
		LoopValidPlayers(i)
			num++;
			
	} else {
	
		LoopValidPlayers(i)
			num++;
	}

	return num;
	
}

stock int GetClientSpecing(int client) {

	if(IsPlayerAlive(client))
		return -1;

	int iSpecMode = GetEntProp(client, Prop_Send, "m_iObserverMode");

	if (iSpecMode != SPECMODE_FIRSTPERSON && iSpecMode != SPECMODE_3RDPERSON)
		return -1;

	int iTarget = GetEntPropEnt(client, Prop_Send, "m_hObserverTarget");

	if(!IsValidClient(iTarget))
		return -1;

	return iTarget;
	
}

stock void SetClientSpectating(int client, int target) {

	if(IsPlayerAlive(client) || !IsPlayerAlive(target))
		return;

	int iSpecMode = GetEntProp(client, Prop_Send, "m_iObserverMode");

	if (iSpecMode != SPECMODE_FIRSTPERSON && iSpecMode != SPECMODE_3RDPERSON)
		SetEntProp(client, Prop_Send, "m_iObserverMode", SPECMODE_FIRSTPERSON);

	SetEntPropEnt(client, Prop_Send, "m_hObserverTarget", target);
	
}

stock int RemoveAllGuns(int client, bool setknife=true) {

	int offset = FindDataMapInfo(client, "m_hMyWeapons");
	
	int numWeaponsRemoved = 0;
	for (int i=0; i < 48; i++){
	
		offset += 4;

		int weapon = GetEntDataEnt2(client, offset);
		
		if (!IsValidEdict(weapon))
			continue;
			
		char entity_className[64];
		GetEntPropString(weapon, Prop_Data, "m_iClassname", entity_className, sizeof(entity_className));
		if (StrContains(entity_className, "weapon_") == -1)
			continue;
		
		FakeClientCommand(client, "use weapon_knife");

		if (RemovePlayerItem(client, weapon)) 
			AcceptEntityInput(weapon, "Kill");

		numWeaponsRemoved++;
		
	}
	
	return numWeaponsRemoved;
	
}

stock float AbsoluteValue(float num) {

	if (num < 0.0)
		num = num * -1.0;

	return num;
	
}

stock bool AddMenuItemFormat(Handle menu, const char[] info, int style = ITEMDRAW_DEFAULT, const char[] format, any ...) {

	char display[128];
	VFormat(display, sizeof(display), format, 5);

	return (AddMenuItem(menu, info, display, style));

}

stock bool CheckAdminFlagsByString(client, const char[] flagString) {

    AdminId admin = GetUserAdmin(client);
    if (admin != INVALID_ADMIN_ID) {
    
        int count, found, flags = ReadFlagString(flagString);
        
        for (int i = 0; i <= 20; i++) {
        
            if (flags & (1<<i)) {
            
                count++;

                if (GetAdminFlag(admin, AdminFlag:i))
                    found++;
                    
            }
            
        }

        if (count == found)
            return true;
            
    }

    return false;
    
} 

stock void GetClientFlagString(int client, char[] flags, int size) {

    int bit = GetUserFlagBits(client);
   
    AdminFlag flagbits[32];
    int num = FlagBitsToArray(bit, flagbits, sizeof(flagbits));
   
    char sBuffer[64];
    for (int i = 0; i < num; i++) {
    
        int char2;
        FindFlagChar(flagbits[i], char2);
        sBuffer[i] = char2;
        
    }
   
    strcopy(flags, size, sBuffer);
    
}

stock int GetTeamAliveCount(int team) {

	int count = 0;

	LoopAlivePlayers(i) {
	
		if (GetClientTeam(i) != team)
			continue;
	
		count++;
	
	}
	
	return count;

}

stock void StripAllWeapons(int client) {

	if (!IsValidClient(client, false))
		return;

	int weapon;
	for (int i; i < 4; i++) {
	
		if ((weapon = GetPlayerWeaponSlot(client, i)) != -1) {
		
			if (IsValidEntity(weapon)) {
			
				RemovePlayerItem(client, weapon);
				AcceptEntityInput(weapon, "Kill");
				
			}
			
		}
		
	}
	
}

stock bool IsFirstDay() {

	if (CS_GetTeamScore(2) == 0 && CS_GetTeamScore(3) == 0)
		return true;

	return false;

}

stock bool VecEqual(float vec1[3], float vec2[3]) {

	if (vec1[0] == vec2[0] && vec1[1] == vec2[1] && vec1[2] == vec2[2])
		return true;
		
	return false;

}

stock void CopyVec(float vec1[3], float vec2[3]) {

	vec2[0] = vec1[0];
	vec2[1] = vec1[1];
	vec2[2] = vec1[2];

	return;

}

stock int GetClientAimTargetPos(int client, float pos[3]) 
{
	if (!client) 
		return -1;
	
	float vAngles[3]; float vOrigin[3];
	
	GetClientEyePosition(client,vOrigin);
	GetClientEyeAngles(client, vAngles);
	
	Handle trace = TR_TraceRayFilterEx(vOrigin, vAngles, MASK_SHOT, RayType_Infinite, TraceFilterAllEntities, client);
	
	TR_GetEndPosition(pos, trace);
	pos[2] += 5.0;
	
	int entity = TR_GetEntityIndex(trace);
	
	CloseHandle(trace);
	
	return entity;
}

stock bool TraceFilterAllEntities(int entity, int contentsMask, any client)
{
	if (entity == client)
		return false;
	if (entity > MaxClients)
		return false;
	if(!IsClientInGame(entity))
		return false;
	if(!IsPlayerAlive(entity))
		return false;
	
	return true;
}

stock void ShowUrl(int client, char[] url) {

	char motdUrl[500];
	Format(motdUrl, 500, "javascript: var x = screen.width * 0.90;var y = screen.height * 0.90;window.open(\"%s\", \"Dope Shit\",\"scrollbars=yes, width='+x+',height='+y+'\");", url); 
	ShowMOTDPanel(client, "Open HTML MOTD", motdUrl, MOTDPANEL_TYPE_URL ); 

}

stock void BlindPlayer(int client, int amount, int[] color, bool hideRadar = false) {

	UserMsg g_FadeUserMsgId = GetUserMessageId("Fade");

	if (g_FadeUserMsgId == INVALID_MESSAGE_ID)
		return;

	if (hideRadar)
		SetEntProp(client, Prop_Send, "m_iHideHUD", GetEntProp(client, Prop_Send, "m_iHideHUD") | HIDEHUD_CSGO_RADAR);

	int targets[2];
	targets[0] = client;

	Handle message = StartMessageEx(g_FadeUserMsgId, targets, 1);

	int clr[4];
	clr[0] = color[0];
	clr[1] = color[1];
	clr[2] = color[2];
	clr[3] = amount;

	Protobuf pb = UserMessageToProtobuf(message);
	pb.SetInt("duration", 600);
	pb.SetInt("hold_time", 1536);
	pb.SetInt("flags", (0x0002 | 0x0008));
	pb.SetColor("clr", clr);

	EndMessage();

}

stock void UnBlindPlayer(int client, bool unHideRadar = false) {

	UserMsg g_FadeUserMsgId = GetUserMessageId("Fade");

	if (g_FadeUserMsgId == INVALID_MESSAGE_ID)
		return;

	if (unHideRadar)
		SetEntProp(client, Prop_Send, "m_iHideHUD", GetEntProp(client, Prop_Send, "m_iHideHUD") & ~HIDEHUD_CSGO_RADAR);

	int targets[2];
	targets[0] = client;

	Handle message = StartMessageEx(g_FadeUserMsgId, targets, 1);

	Protobuf pb = UserMessageToProtobuf(message);
	pb.SetInt("duration", 1);
	pb.SetInt("hold_time", 1536);
	pb.SetInt("flags", (0x0001 | 0x0010));
	pb.SetColor("clr", { 0, 0, 0, 0 });

	EndMessage();

}

stock int GetNearestPlayer(int client, int team) {

	float clientOrigin[3], iOrigin[3];
	
	int nearest = 0;
	float last = 0.0;
	
	GetClientAbsOrigin(client, clientOrigin);
	
	LoopAlivePlayers(i) {
		
		if (GetClientTeam(i) == team) {
		
			GetClientAbsOrigin(i, iOrigin);
			float distance = GetVectorDistance(clientOrigin, iOrigin);
	
			if (last == 0.0) {
			
				last = distance;
				nearest = i;
				
			}
	
			if (distance < last) {
			
				last = distance;
				nearest = i;
				
			}
			
		}
		
	}
	
	return nearest;
	
}

stock bool IsClientTooNearObstacle(int client, float distance = 110.0, bool checkAllEntities = false)
{
	float posEye[3], posEyeAngles[3], vecOrigin[3], vecOrigin_edict[3];
	char Class_edict[64];
	bool isClientLookingAtWall = false, isClientCloseToEdict = false;
	
	GetClientAbsOrigin(client, vecOrigin);
	
	if (checkAllEntities) {
	
		for (int edict = 1; edict <= GetMaxEntities(); edict++) {
			
			if (!IsValidEntity(edict))
				continue;

			GetEdictClassname(edict, Class_edict, sizeof(Class_edict));

			if (StrContains(Class_edict, "player") != -1)
				continue;

			if (StrContains(Class_edict, "weapon") != -1)
				continue;

			if (GetEntSendPropOffs(edict, "m_vecOrigin") == -1)
				continue;

			GetEntPropVector(edict, Prop_Data, "m_vecOrigin", vecOrigin_edict);

			float edict_distance = GetVectorDistance(vecOrigin, vecOrigin_edict);

			if (edict_distance <= distance)
				isClientCloseToEdict = true;

		}
	}

	GetClientEyePosition(client, posEye);
	GetClientEyeAngles(client,	posEyeAngles);

	posEyeAngles[0] = 0.0;

	Handle trace = TR_TraceRayFilterEx(posEye, posEyeAngles, CONTENTS_SOLID, RayType_Infinite, Trace_FilterNothing);
	
	if (TR_DidHit(trace))
	{
		float posEnd[3];

		TR_GetEndPosition(posEnd, trace);
		
		if (GetVectorDistance(posEye, posEnd) <= distance)
		{
			float m_vHullMin[3] = {-16.0, -16.0, 0.0};
			float m_vHullMax[3] = {16.0, 16.0, 72.0};
			
			Handle hullTrace = 	TR_TraceHullEx(posEye, posEnd, m_vHullMin, m_vHullMax, CONTENTS_SOLID);

			if (TR_DidHit(hullTrace)) {

				TR_GetEndPosition(posEnd, hullTrace);

				if (GetVectorDistance(posEye, posEnd) <= distance)
					isClientLookingAtWall = true;

			}

			CloseHandle(hullTrace);

		}

	}

	CloseHandle(trace);	

	if (isClientCloseToEdict || isClientLookingAtWall)
		return true;

	return false;
}

public bool Trace_FilterNothing(int entity, int contentsMask) {

	return entity == 0;

}

stock BlockEntity(int client, int cachedOffset) {

	SetEntData(client, cachedOffset, 5, 4, true);
	
}

stock UnblockEntity(int client, int cachedOffset) {

	SetEntData(client, cachedOffset, 2, 4, true);
	
}

stock void FreezePlayer(int client) {

	SetEntityMoveType(client, MOVETYPE_NONE);
	//SetEntityRenderColor(client, 0, 128, 255, 135);

}

stock void UnFreezePlayer(int client) {

	SetEntityMoveType(client, MOVETYPE_WALK);
	//SetEntityRenderColor(client, 255, 255, 255, 255);

}