/*
    INDI LibCamera Driver

    Copyright (C) 2022 Jasem Mutlaq (mutlaqja@ikarustech.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include "indipropertyswitch.h"
//#include "indipropertynumber.h"
//#include "indipropertytext.h"
#include "indisinglethreadpool.h"

#include "core/libcamera_app.hpp"
#include "core/still_options.hpp"

#include <vector>

#include <indiccd.h>
#include <inditimer.h>

class SingleWorker;
class INDILibCamera : public INDI::CCD, public LibcameraApp
{
    public:
        INDILibCamera();
        ~INDILibCamera() override;

        virtual const char *getDefaultName() override;

        virtual bool initProperties() override;
        virtual bool updateProperties() override;

        virtual bool Connect() override;
        virtual bool Disconnect() override;

        virtual bool StartExposure(float duration) override;
        virtual bool AbortExposure() override;

        StillOptions *GetOptions() const { return static_cast<StillOptions *>(options_.get()); }

    protected:

        virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) override;
        virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;

        // Streaming
        virtual bool StartStreaming() override;
        virtual bool StopStreaming() override;

        virtual bool UpdateCCDFrame(int x, int y, int w, int h) override;
        virtual bool UpdateCCDBin(int binx, int biny) override;

        // specific keywords
        virtual void addFITSKeywords(INDI::CCDChip *targetChip) override;

        // Save config
        virtual bool saveConfigItems(FILE *fp) override;        

        /** Get the current Bayer string used */
        const char *getBayerString() const;

    protected:
        INDI::SingleThreadPool m_Worker;
        void workerStreamVideo(const std::atomic_bool &isAboutToQuit);
        void workerExposure(const std::atomic_bool &isAboutToQuit, float duration);

    protected:
        /** Get initial parameters from camera */
        void setup();

        /** Update SER recorder video format */
        void updateRecorderFormat();

        enum
        {
            CAPTURE_DNG,
            CAPTURE_JPG
        };

        bool processRAW(const char *filename, uint8_t **memptr, size_t *memsize, int *n_axis, int *w, int *h, int *bitsperpixel,
                        char *bayer_pattern);

        bool processJPEG(const char *filename, uint8_t **memptr, size_t *memsize, int *naxis, int *w, int *h);

     private:
        std::unique_ptr<CameraManager> m_CameraManager;

};
