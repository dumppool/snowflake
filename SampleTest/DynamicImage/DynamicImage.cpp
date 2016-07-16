// Fill out your copyright notice in the Description page of Project Settings.

#include "XYVR410.h"
#include "DynamicImage.h"

FLVDynamicImage::FLVDynamicImage()
{

}

FLVDynamicImage::~FLVDynamicImage()
{

}

bool FLVDynamicImage::Init()
{
	return true;
}

uint32 FLVDynamicImage::Run()
{
	return 0;
}

void FLVDynamicImage::Stop()
{

}

void FLVDynamicImage::Exit()
{

}

void FLVDynamicImage::SetImageUrl(const FString& InUrl)
{
	if (InUrl == UrlLoaded && !InUrl.IsEmpty())
	{
		if (OnLoaded.IsBound())
		{
			OnLoaded.Execute();
		}

		return;
	}

	if (UrlWanted.IsEmpty())
	{

	}
}