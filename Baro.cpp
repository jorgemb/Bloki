#include "Baro.h"

#include "resources.h"
#include "GdiUtility.h"
#include "Utility.h"

Baro* Baro::s_pBaro = NULL;
const INT BaroSize = 256;

Baro::Baro( Renderer* pRenderer ) :
m_pRenderer( pRenderer ),
m_Mood( MOOD_NORMAL ),
m_Animation( ANIMATION_NONE ),
m_Position( 0, 0 ),
m_LeftHandPos( 0 ),
m_RightHandPos( 0 ),
m_CurrentAnimationTime( 0 )
{
	if( s_pBaro )
		throw BaroObjAlreadyInitialized();
	else
		s_pBaro = this;

	// Load all baro bitmaps
	UINT currentImage = 0;
	while( currentImage != BARO_IMAGES )
	{
		Gdiplus::Bitmap* pImage = NULL;
		pImage = loadGdiBitmap( IndexToResource( currentImage ), PNG_TYPE );
		if( !pImage )
			throw Ludos::Exception( "Couldn't load Baro image.", "Baro" );

		m_Images.push_back( pImage );
		++currentImage;
	}
}

Baro::~Baro()
{
	s_pBaro = NULL;

	// Unload all baro bitmaps
	for( iterImage pIter = m_Images.begin(); pIter != m_Images.end(); ++pIter )
	{
		Gdiplus::Bitmap* pImage = *pIter;
		S_DELETE( pImage );
	}
	m_Images.clear();
}

Baro* Baro::getSingletonPtr()
{
	if( !s_pBaro )
		throw InvalidBaroObj();
	else
		return s_pBaro;
}

Baro& Baro::getSingleton()
{
	if( !s_pBaro )
		throw InvalidBaroObj();
	else
		return *s_pBaro;
}


VOID Baro::Update( DWORD timeDelta )
{
	const INT HandMovementLimit = 20;
	const DOUBLE HandMovementSpeed = ( HandMovementLimit / 0.15 ) / 1000.0;
	BOOL WaveBothHands = FALSE;

	static DOUBLE MoveDelta = HandMovementSpeed;
	if( ( abs( m_LeftHandPos ) >= HandMovementLimit ) || 
		( abs( m_RightHandPos ) >= HandMovementLimit ) )
	{
		MoveDelta = -MoveDelta;
		
		if( m_LeftHandPos > 0 )
			m_LeftHandPos = HandMovementLimit;
		else if( m_LeftHandPos < 0 )
			m_LeftHandPos = -HandMovementLimit;

		if( m_RightHandPos > 0 )
			m_RightHandPos = HandMovementLimit;
		else if( m_RightHandPos < 0 )
			m_RightHandPos = -HandMovementLimit;
	}

	switch( m_Animation )
	{
	case( ANIMATION_NONE ):
		m_LeftHandPos = 0;
		m_RightHandPos = 0;
		m_CurrentAnimationTime = 0;
		break;

	case( ANIMATION_WAVE ):
		WaveBothHands = TRUE;
	case( ANIMATION_WAVE_LEFT ):
		m_LeftHandPos += MoveDelta * timeDelta;
		if( !WaveBothHands ) break;
	case( ANIMATION_WAVE_RIGHT ):
		m_RightHandPos += MoveDelta * timeDelta;
		break;
	}

	// Update animation time, and return to the NONE animation when it runs out
	if( m_CurrentAnimationTime > 0 )
		m_CurrentAnimationTime -= timeDelta;
	if( m_CurrentAnimationTime <= 0 )
		setAnimation( ANIMATION_NONE, 0 );


	m_pRenderer->addRenderObj( this );
}

BOOL Baro::Draw( Gdiplus::Graphics *pGraphics )
{
	// In layers it should go like this: Left arm, legs, body, right arm, eyes and mouth
	drawImage( pGraphics, BARO_LEFT_ARM, (INT)floor( m_LeftHandPos ) );
	drawImage( pGraphics, BARO_LEGS );
	drawImage( pGraphics, BARO_BODY );
	drawImage( pGraphics, BARO_RIGHT_ARM, (INT)floor( m_RightHandPos ) );

	switch( m_Mood )
	{
	case MOOD_NORMAL:
		drawImage( pGraphics, BARO_EYES_NORMAL );
		drawImage( pGraphics, BARO_MOUTH_HAPPY );
		break;
	case MOOD_HAPPY:
		drawImage( pGraphics, BARO_EYES_HAPPY );
		drawImage( pGraphics, BARO_MOUTH_HAPPY );
		break;
	case MOOD_WORRIED:
		drawImage( pGraphics, BARO_EYES_WORRIED );
		drawImage( pGraphics, BARO_MOUTH_WORRIED );
		break;
	case MOOD_SAD:
		drawImage( pGraphics, BARO_EYES_WORRIED );
		drawImage( pGraphics, BARO_MOUTH_SAD );
		break;
	case MOOD_MAD:
		drawImage( pGraphics, BARO_EYES_MAD );
		drawImage( pGraphics, BARO_MOUTH_SAD );
		break;
	}

	return TRUE;
}

VOID Baro::setMood( MOOD Mood )
{
	m_Mood = Mood;
}

MOOD Baro::getMood()
{
	return m_Mood;
}

VOID Baro::setAnimation( ANIMATION Animation, DWORD TimeMS )
{
	m_Animation = Animation;
	m_CurrentAnimationTime = TimeMS;

	m_LeftHandPos = 0;
	m_RightHandPos = 0;
}

ANIMATION Baro::getAnimation()
{
	return m_Animation;
}

VOID Baro::setPosition( const Gdiplus::Point& Position )
{
	m_Position = Position;
}

Gdiplus::Point Baro::getPosition()
{
	return m_Position;
}

UINT Baro::ResourceToIndex( UINT Resource )
{
	return Resource - BARO_FIRST;
}

UINT Baro::IndexToResource( UINT Index )
{
	return Index + BARO_FIRST;
}

VOID Baro::drawImage( Gdiplus::Graphics* pGraphics, UINT Resource, INT RelativePos )
{
	pGraphics->DrawImage( m_Images[ ResourceToIndex( Resource ) ],
		m_Position.X, m_Position.Y + RelativePos,
		BaroSize, BaroSize );
}