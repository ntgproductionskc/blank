#include "MissionSystem.h"
#include "../Core/SupermanLoisGameMode.h"
#include "../Core/SupermanLoisGameState.h"
#include "../Systems/ReputationSystem.h"
#include "Kismet/GameplayStatics.h"

void UMissionSystem::Initialize(ASupermanLoisGameMode* GameMode)
{
	OwningGameMode = GameMode;
	PopulateStoryMissions();
}

void UMissionSystem::PopulateStoryMissions()
{
	// ── Chapter 1: A New Threat ───────────────────────────────────────────────
	{
		FMissionData M;
		M.MissionID         = FName("CH1_M1_Arrival");
		M.Title             = FText::FromString(TEXT("Welcome to Metropolis"));
		M.Description       = FText::FromString(TEXT("Return to Metropolis after a threat call from the DOD. Something is wrong with the city's power grid."));
		M.Type              = EMissionType::Story;
		M.Chapter           = 1;
		M.ReputationReward  = 5.f;
		M.bHasCutscene      = true;
		M.StartCutsceneID   = FName("CS_CH1_Open");
		M.NextMissionID     = FName("CH1_M2_PowerGrid");

		FMissionObjective Obj1;
		Obj1.Description  = FText::FromString(TEXT("Fly to the Daily Planet"));
		Obj1.Type         = EObjectiveType::ReachLocation;
		Obj1.TargetLocation = FVector(-5000.f, 2000.f, 0.f); // Daily Planet coords
		Obj1.RequiredCount = 1;
		M.Objectives.Add(Obj1);

		AllMissions.Add(M);
		MissionStatusMap.Add(M.MissionID, EMissionStatus::Available);
	}
	{
		FMissionData M;
		M.MissionID         = FName("CH1_M2_PowerGrid");
		M.Title             = FText::FromString(TEXT("Lights Out"));
		M.Description       = FText::FromString(TEXT("Metropolis's power grid is failing — someone is sabotaging the substations. Protect the city."));
		M.Type              = EMissionType::Story;
		M.Chapter           = 1;
		M.ReputationReward  = 8.f;
		M.NextMissionID     = FName("CH1_M3_MorganEdge");
		M.PrerequisiteMissions.Add(FName("CH1_M1_Arrival"));

		FMissionObjective Obj1;
		Obj1.Description   = FText::FromString(TEXT("Defend 3 power substations"));
		Obj1.Type          = EObjectiveType::ProtectTarget;
		Obj1.RequiredCount = 3;
		M.Objectives.Add(Obj1);

		FMissionObjective Obj2;
		Obj2.Description   = FText::FromString(TEXT("Defeat the saboteurs"));
		Obj2.Type          = EObjectiveType::DefeatEnemy;
		Obj2.RequiredCount = 8;
		M.Objectives.Add(Obj2);

		AllMissions.Add(M);
		MissionStatusMap.Add(M.MissionID, EMissionStatus::Locked);
	}
	{
		FMissionData M;
		M.MissionID         = FName("CH1_M3_MorganEdge");
		M.Title             = FText::FromString(TEXT("The Man Behind the Curtain"));
		M.Description       = FText::FromString(TEXT("Morgan Edge — Tal-Rho — has resurfaced. His X-Kryptonite operation is threatening Smallville. Clark must stop him."));
		M.Type              = EMissionType::Story;
		M.Chapter           = 1;
		M.ReputationReward  = 15.f;
		M.bHasCutscene      = true;
		M.StartCutsceneID   = FName("CS_MorganEdge_Reveal");
		M.EndCutsceneID     = FName("CS_MorganEdge_Defeated");
		M.NextMissionID     = FName("CH2_M1_BizarroWorld");
		M.PrerequisiteMissions.Add(FName("CH1_M2_PowerGrid"));

		FMissionObjective Obj1;
		Obj1.Description   = FText::FromString(TEXT("Reach Smallville before Morgan Edge's soldiers lock it down"));
		Obj1.Type          = EObjectiveType::ReachLocation;
		Obj1.RequiredCount = 1;
		M.Objectives.Add(Obj1);

		FMissionObjective Obj2;
		Obj2.Description   = FText::FromString(TEXT("Rescue X-Kryptonite victims"));
		Obj2.Type          = EObjectiveType::RescueCivilian;
		Obj2.RequiredCount = 10;
		M.Objectives.Add(Obj2);

		FMissionObjective Obj3;
		Obj3.Description   = FText::FromString(TEXT("Defeat Morgan Edge / Tal-Rho"));
		Obj3.Type          = EObjectiveType::DefeatEnemy;
		Obj3.RequiredCount = 1;
		Obj3.TargetTag     = FName("MorganEdge");
		M.Objectives.Add(Obj3);

		AllMissions.Add(M);
		MissionStatusMap.Add(M.MissionID, EMissionStatus::Locked);
	}

	// ── Chapter 2: Bizarro World ──────────────────────────────────────────────
	{
		FMissionData M;
		M.MissionID         = FName("CH2_M1_BizarroWorld");
		M.Title             = FText::FromString(TEXT("Mirror Image"));
		M.Description       = FText::FromString(TEXT("A portal opens over Metropolis — Bizarro and forces from the Inverse World pour through. The city cannot hold."));
		M.Type              = EMissionType::Story;
		M.Chapter           = 2;
		M.ReputationReward  = 20.f;
		M.bHasCutscene      = true;
		M.StartCutsceneID   = FName("CS_BizarroPortal");
		M.NextMissionID     = FName("CH2_M2_InverseWorld");
		M.PrerequisiteMissions.Add(FName("CH1_M3_MorganEdge"));

		FMissionObjective Obj1;
		Obj1.Description   = FText::FromString(TEXT("Close 3 portals over Metropolis"));
		Obj1.Type          = EObjectiveType::DestroyObject;
		Obj1.RequiredCount = 3;
		Obj1.TargetTag     = FName("InversePortal");
		M.Objectives.Add(Obj1);

		FMissionObjective Obj2;
		Obj2.Description   = FText::FromString(TEXT("Defeat Bizarro"));
		Obj2.Type          = EObjectiveType::DefeatEnemy;
		Obj2.RequiredCount = 1;
		Obj2.TargetTag     = FName("Bizarro");
		M.Objectives.Add(Obj2);

		FMissionObjective Obj3;
		Obj3.Description   = FText::FromString(TEXT("Save Lois from Bizarro's freeze vision"));
		Obj3.Type          = EObjectiveType::RescueCivilian;
		Obj3.RequiredCount = 1;
		Obj3.TargetTag     = FName("LoisLane");
		Obj3.bOptional     = false;
		M.Objectives.Add(Obj3);

		AllMissions.Add(M);
		MissionStatusMap.Add(M.MissionID, EMissionStatus::Locked);
	}
	{
		FMissionData M;
		M.MissionID         = FName("CH2_M2_InverseWorld");
		M.Title             = FText::FromString(TEXT("Into the Inverse"));
		M.Description       = FText::FromString(TEXT("Superman must enter the Inverse World to defeat the Inverse Superman before he crosses over permanently."));
		M.Type              = EMissionType::Story;
		M.Chapter           = 2;
		M.ReputationReward  = 25.f;
		M.bHasCutscene      = true;
		M.StartCutsceneID   = FName("CS_EnterInverse");
		M.EndCutsceneID     = FName("CS_InverseDefeated");
		M.NextMissionID     = FName("CH3_M1_Doomsday");
		M.PrerequisiteMissions.Add(FName("CH2_M1_BizarroWorld"));

		FMissionObjective Obj1;
		Obj1.Description   = FText::FromString(TEXT("Navigate the Inverse World to find the Inverse Superman"));
		Obj1.Type          = EObjectiveType::ReachLocation;
		Obj1.RequiredCount = 1;
		M.Objectives.Add(Obj1);

		FMissionObjective Obj2;
		Obj2.Description   = FText::FromString(TEXT("Defeat the Inverse Superman"));
		Obj2.Type          = EObjectiveType::DefeatEnemy;
		Obj2.RequiredCount = 1;
		Obj2.TargetTag     = FName("InverseSuperman");
		M.Objectives.Add(Obj2);

		AllMissions.Add(M);
		MissionStatusMap.Add(M.MissionID, EMissionStatus::Locked);
	}

	// ── Chapter 3: Doomsday Rising ───────────────────────────────────────────
	{
		FMissionData M;
		M.MissionID         = FName("CH3_M1_Doomsday");
		M.Title             = FText::FromString(TEXT("The End of All Things"));
		M.Description       = FText::FromString(TEXT("Doomsday — an unstoppable Kryptonian weapon — is unleashed on Metropolis. No army can stop it. Only Superman can."));
		M.Type              = EMissionType::Story;
		M.Chapter           = 3;
		M.ReputationReward  = 50.f;
		M.bHasCutscene      = true;
		M.StartCutsceneID   = FName("CS_DoomsdayRising");
		M.EndCutsceneID     = FName("CS_SupermanVictory");
		M.PrerequisiteMissions.Add(FName("CH2_M2_InverseWorld"));

		FMissionObjective Obj1;
		Obj1.Description   = FText::FromString(TEXT("Evacuate Metropolis civilians before Doomsday reaches downtown"));
		Obj1.Type          = EObjectiveType::RescueCivilian;
		Obj1.RequiredCount = 50;
		M.Objectives.Add(Obj1);

		FMissionObjective Obj2;
		Obj2.Description   = FText::FromString(TEXT("Survive Doomsday's first wave"));
		Obj2.Type          = EObjectiveType::SurviveWaves;
		Obj2.RequiredCount = 3;
		M.Objectives.Add(Obj2);

		FMissionObjective Obj3;
		Obj3.Description   = FText::FromString(TEXT("Defeat Doomsday"));
		Obj3.Type          = EObjectiveType::DefeatEnemy;
		Obj3.RequiredCount = 1;
		Obj3.TargetTag     = FName("Doomsday");
		M.Objectives.Add(Obj3);

		AllMissions.Add(M);
		MissionStatusMap.Add(M.MissionID, EMissionStatus::Locked);
	}

	// ── Side missions ──────────────────────────────────────────────────────────
	{
		FMissionData M;
		M.MissionID        = FName("SIDE_JordanPowers");
		M.Title            = FText::FromString(TEXT("Like Father, Like Son"));
		M.Description      = FText::FromString(TEXT("Jordan's powers are surging beyond his control. Clark needs to train him before someone gets hurt."));
		M.Type             = EMissionType::Companion;
		M.ReputationReward = 8.f;

		FMissionObjective Obj1;
		Obj1.Description   = FText::FromString(TEXT("Train Jordan in heat vision control"));
		Obj1.Type          = EObjectiveType::UseAbility;
		Obj1.RequiredCount = 5;
		Obj1.TargetTag     = FName("HeatVisionTarget");
		M.Objectives.Add(Obj1);

		AllMissions.Add(M);
		MissionStatusMap.Add(M.MissionID, EMissionStatus::Available);
	}
	{
		FMissionData M;
		M.MissionID        = FName("SIDE_LoisInvestigates");
		M.Title            = FText::FromString(TEXT("Follow the Money"));
		M.Description      = FText::FromString(TEXT("Lois is investigating Lex Luthor's shell companies in Metropolis. She's in danger. Protect her without revealing your identity."));
		M.Type             = EMissionType::Companion;
		M.ReputationReward = 10.f;

		FMissionObjective Obj1;
		Obj1.Description   = FText::FromString(TEXT("Shadow Lois through Metropolis undetected"));
		Obj1.Type          = EObjectiveType::EscortTarget;
		Obj1.RequiredCount = 1;
		Obj1.TargetTag     = FName("LoisLane");
		M.Objectives.Add(Obj1);

		FMissionObjective Obj2;
		Obj2.Description   = FText::FromString(TEXT("Neutralize assassins sent after Lois"));
		Obj2.Type          = EObjectiveType::DefeatEnemy;
		Obj2.RequiredCount = 6;
		M.Objectives.Add(Obj2);

		AllMissions.Add(M);
		MissionStatusMap.Add(M.MissionID, EMissionStatus::Available);
	}
}

void UMissionSystem::StartMission(FName MissionID)
{
	for (FMissionData& M : AllMissions)
	{
		if (M.MissionID == MissionID)
		{
			if (!ArePrerequisitesMet(M)) return;

			MissionStatusMap.FindOrAdd(MissionID) = EMissionStatus::Active;
			ActiveMissionID = MissionID;
			OnMissionStarted.Broadcast(M);
			return;
		}
	}
}

void UMissionSystem::UpdateObjective(FName MissionID, EObjectiveType ObjectiveType, int32 CountDelta)
{
	for (FMissionData& M : AllMissions)
	{
		if (M.MissionID != MissionID) continue;

		for (FMissionObjective& Obj : M.Objectives)
		{
			if (Obj.Type == ObjectiveType && !Obj.bCompleted)
			{
				Obj.CurrentCount = FMath::Min(Obj.CurrentCount + CountDelta, Obj.RequiredCount);
				if (Obj.CurrentCount >= Obj.RequiredCount)
					Obj.bCompleted = true;

				OnObjectiveUpdated.Broadcast(MissionID, Obj);
				CheckObjectiveCompletion(MissionID);
				return;
			}
		}
	}
}

void UMissionSystem::CheckObjectiveCompletion(FName MissionID)
{
	for (const FMissionData& M : AllMissions)
	{
		if (M.MissionID != MissionID) continue;

		bool bAllDone = true;
		for (const FMissionObjective& Obj : M.Objectives)
		{
			if (!Obj.bOptional && !Obj.bCompleted)
			{
				bAllDone = false;
				break;
			}
		}

		if (bAllDone) CompleteMission(MissionID, true);
		return;
	}
}

void UMissionSystem::CompleteMission(FName MissionID, bool bSuccess)
{
	MissionStatusMap.FindOrAdd(MissionID) = bSuccess ? EMissionStatus::Completed : EMissionStatus::Failed;

	if (bSuccess)
	{
		// Unlock next mission
		for (const FMissionData& M : AllMissions)
		{
			if (M.MissionID == MissionID && !M.NextMissionID.IsNone())
			{
				MissionStatusMap.FindOrAdd(M.NextMissionID) = EMissionStatus::Available;
				break;
			}
		}

		// Award reputation
		if (OwningGameMode)
		{
			for (const FMissionData& M : AllMissions)
			{
				if (M.MissionID == MissionID)
				{
					// Reputation handled by ReputationSystem via GameMode
					break;
				}
			}
		}

		// Update game state
		if (OwningGameMode)
		{
			if (ASupermanLoisGameState* GS = Cast<ASupermanLoisGameState>(
				OwningGameMode->GetWorld()->GetGameState()))
			{
				GS->CompletedMissions.AddUnique(MissionID);
			}
		}
	}

	ActiveMissionID = NAME_None;
	OnMissionCompleted.Broadcast(MissionID, bSuccess);
}

FMissionData* UMissionSystem::GetActiveMission() const
{
	for (FMissionData& M : const_cast<UMissionSystem*>(this)->AllMissions)
	{
		if (M.MissionID == ActiveMissionID) return &M;
	}
	return nullptr;
}

TArray<FMissionData*> UMissionSystem::GetAvailableMissions() const
{
	TArray<FMissionData*> Available;
	for (FMissionData& M : const_cast<UMissionSystem*>(this)->AllMissions)
	{
		const EMissionStatus* Status = MissionStatusMap.Find(M.MissionID);
		if (Status && *Status == EMissionStatus::Available)
			Available.Add(&M);
	}
	return Available;
}

bool UMissionSystem::IsMissionComplete(FName MissionID) const
{
	const EMissionStatus* Status = MissionStatusMap.Find(MissionID);
	return Status && *Status == EMissionStatus::Completed;
}

bool UMissionSystem::ArePrerequisitesMet(const FMissionData& Mission) const
{
	for (const FName& Prereq : Mission.PrerequisiteMissions)
	{
		if (!IsMissionComplete(Prereq)) return false;
	}
	return true;
}

void UMissionSystem::Tick(float DeltaTime)
{
	// Check time-limit objectives etc. — extended here
}
