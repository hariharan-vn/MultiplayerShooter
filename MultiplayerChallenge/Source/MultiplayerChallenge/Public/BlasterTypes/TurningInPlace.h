// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class ETurningInPlace :uint8
{
	ETIP_Left UMETA(DisplayName = "Turn Left"),
	ETIP_Right UMETA(DisplayName = "Turn Right"),
	ETIP_NotTurning UMETA(DisplayName = "Not Turning"),

	ETIP_MAX UMETA(DisplayName = "Default Max")
};