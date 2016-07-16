// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DECLARE_DELEGATE(FOnDynamicImageLoaded);

class FLVDynamicImage : public FRunnable
{
public:
	FLVDynamicImage();
	~FLVDynamicImage();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	FOnDynamicImageLoaded OnLoaded;

	void SetImageUrl(const FString& InUrl);
	const UTexture2D* GetTexture2D() const { return Res; }

protected:

	UTexture2D*		Res;
	FString			UrlLoaded;
	FString			UrlWanted;

	TAutoPtr<FEvent>			StopEvent;
	TAutoPtr<FEvent>			WaitEvent;
	TAutoPtr<FRunnableThread>	Thread;
};
