#include "AppController.h"


// =============================================================
// Personal
// =============================================================

void AppController::loadMyWave()
{
    if (
        m_personalController == nullptr
    )
    {
        return;
    }


    m_personalController
        ->loadMyWave();
}


void AppController::loadMoreMyWave()
{
    if (
        m_personalController == nullptr
    )
    {
        return;
    }


    m_personalController
        ->loadMoreMyWave();
}


void AppController::loadRecommendations()
{
    if (
        m_personalController == nullptr
    )
    {
        return;
    }


    m_personalController
        ->loadRecommendations();
}