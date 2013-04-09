/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://www.xbmc.org
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#pragma once

#include "VAAPI.h"
#include "threads/CriticalSection.h"

#include <boost/shared_ptr.hpp>
#include <vector>
#include <list>

namespace VAAPI
{
    struct CVPPPicture
    {
      CVPPPicture():valid(false) {}

      bool valid;
      DVDVideoPicture DVDPic;
      CSurfacePtr surface;
    };

    class CVPP
    {
        CVPP();

        public:
        enum DeintMethod
        {
            DeinterlacingWeave = 0,
            DeinterlacingBob,
            DeinterlacingMotionAdaptive,
            DeinterlacingMotionCompensated,
            Deinterlacing_Count
        };
        enum ReadyState
        {
            NotReady = 0,
            Ready,
            InitFailed,
            RuntimeFailed
        };
        enum SupportState
        {
            Unknown = 0,
            Supported,
            Unsupported
        };

        CVPP(CDisplayPtr& display, int width, int height);
        ~CVPP();

        static bool VppSupported();
        static bool DeintSupported(CVPP::DeintMethod method);

        void deinit();

        bool InitVpp();
        bool InitDeint(CVPP::DeintMethod method, int num_surfaces);

        inline bool VppReady() { return m_vppReady == Ready; }
        inline bool VppFailed() { return m_vppReady == InitFailed || m_vppReady == RuntimeFailed; }
        inline bool DeintReady() { return m_deintReady == Ready; }
        inline bool DeintFailed() { return m_deintReady == InitFailed || m_deintReady == RuntimeFailed; }
        inline DeintMethod getUsedMethod() { return m_usedMethod; }

        CVPPPicture DoDeint(const CVPPPicture& input, int topField = -1);

        private:
        CSurfacePtr getFreeSurface();


        static SupportState g_supported;
        static SupportState g_deintSupported[CVPP::Deinterlacing_Count];


        DeintMethod m_usedMethod;
        unsigned int m_forwardReferencesCount;
        unsigned int m_backwardReferencesCount;
        std::list<CVPPPicture> m_forwardReferences;
        std::list<CVPPPicture> m_backwardReferences;

        CDisplayPtr m_display;
        int m_width;
        int m_height;

        VAConfigID m_configId;

        ReadyState m_vppReady;
        ReadyState m_deintReady;

        //VPP Deinterlacing
        CCriticalSection m_deint_lock;
        VAContextID m_deintContext;
        std::vector<CSurfacePtr> m_deintTargets;
        VABufferID m_deintFilter;
    };

}

