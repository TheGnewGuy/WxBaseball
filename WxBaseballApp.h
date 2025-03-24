/***************************************************************
 * Name:      WxBaseballApp.h
 * Purpose:   Defines Application Class
 * Author:    John Gnew (john.gnew@verizon.net)
 * Created:   2024-03-22
 * Copyright: John Gnew ()
 * License:
 **************************************************************/

#ifndef WXBASEBALLAPP_H
#define WXBASEBALLAPP_H

#include <wx/wx.h>
#include <wx/frame.h>

#include <wx/app.h>
#include <wx/cmndata.h>

#include "WxBaseballMain.h"
#include "DBRoutines.h"
#include "FileRoutines.h"
#include "dialogs.h"

//#define    APP_VERSION      111130
// Julian 336 = 12/2/24 wxDateTime::GetJulianDayNumber()
// Julian 356 = 12/22/24
//JULIAN DATE CALENDAR
//Day Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec Day
// 1  001 032 060 091 121 152 182 213 244 274 305 335  1
// 2  002 033 061 092 122 153 183 214 245 275 306 336  2
// 3  003 034 062 093 123 154 184 215 246 276 307 337  3
// 4  004 035 063 094 124 155 185 216 247 277 308 338  4
// 5  005 036 064 095 125 156 186 217 248 278 309 339  5
// 6  006 037 065 096 126 157 187 218 249 279 310 340  6
// 7  007 038 066 097 127 158 188 219 250 280 311 341  7
// 8  008 039 067 098 128 159 189 220 251 281 312 342  8
// 9  009 040 068 099 129 160 190 221 252 282 313 343  9
//10  010 041 069 100 130 161 191 222 253 283 314 344 10
//11  011 042 070 101 131 162 192 223 254 284 315 345 11
//12  012 043 071 102 132 163 193 224 255 285 316 346 12
//13  013 044 072 103 133 164 194 225 256 286 317 347 13
//14  014 045 073 104 134 165 195 226 257 287 318 348 14
//15  015 046 074 105 135 166 196 227 258 288 319 349 15
//16  016 047 075 106 136 167 197 228 259 289 320 350 16
//17  017 048 076 107 137 168 198 229 260 290 321 351 17
//18  018 049 077 108 138 169 199 230 261 291 322 352 18
//19  019 050 078 109 139 170 200 231 262 292 323 353 19
//20  020 051 079 110 140 171 201 232 263 293 324 354 20
//21  021 052 080 111 141 172 202 233 264 294 325 355 21
//22  022 053 081 112 142 173 203 234 265 295 326 356 22
//23  023 054 082 113 143 174 204 235 266 296 327 357 23
//24  024 055 083 114 144 175 205 236 267 297 328 358 24
//25  025 056 084 115 145 176 206 237 268 298 329 359 25
//26  026 057 085 116 146 177 207 238 269 299 330 360 26
//27  027 058 086 117 147 178 208 239 270 300 331 361 27
//28  028 059 087 118 148 179 209 240 271 301 332 362 28
//29  029     088 119 149 180 210 241 272 302 333 363 29
//30  030     089 120 150 181 211 242 273 303 334 364 30
//31  031     090     151     212 243     304     365 31
//
//  JULIAN DATE CALENDAR Leap Years
//  USE IN 2004, 2008, 2012, 2016, 2020, 2024, 2028, 2032.
//Day Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec Day
// 1  001 032 061 092 122 153 183 214 245 275 306 336  1
// 2  002 033 062 093 123 154 184 215 246 276 307 337  2
// 3  003 034 063 094 124 155 185 216 247 277 308 338  3
// 4  004 035 064 095 125 156 186 217 248 278 309 339  4
// 5  005 036 065 096 126 157 187 218 249 279 310 340  5
// 6  006 037 066 097 127 158 188 219 250 280 311 341  6
// 7  007 038 067 098 128 159 189 220 251 281 312 342  7
// 8  008 039 068 099 129 160 190 221 252 282 313 343  8
// 9  009 040 069 100 130 161 191 222 253 283 314 344  9
//10  010 041 070 101 131 162 192 223 254 284 315 345 10
//11  011 042 071 102 132 163 193 224 255 285 316 346 11
//12  012 043 072 103 133 164 194 225 256 286 317 347 12
//13  013 044 073 104 134 165 195 226 257 287 318 348 13
//14  014 045 074 105 135 166 196 227 258 288 319 349 14
//15  015 046 075 106 136 167 197 228 259 289 320 350 15
//16  016 047 076 107 137 168 198 229 260 290 321 351 16
//17  017 048 077 108 138 169 199 230 261 291 322 352 17
//18  018 049 078 109 139 170 200 231 262 292 323 353 18
//19  019 050 079 110 140 171 201 232 263 293 324 354 19
//20  020 051 080 111 141 172 202 233 264 294 325 355 20
//21  021 052 081 112 142 173 203 234 265 295 326 356 21
//22  022 053 082 113 143 174 204 235 266 296 327 357 22
//23  023 054 083 114 144 175 205 236 267 297 328 358 23
//24  024 055 084 115 145 176 206 237 268 298 329 359 24
//25  025 056 085 116 146 177 207 238 269 299 330 360 25
//26  026 057 086 117 147 178 208 239 270 300 331 361 26
//27  027 058 087 118 148 179 209 240 271 301 332 362 27
//28  028 059 088 119 149 180 210 241 272 302 333 363 28
//29  029 060 089 120 150 181 211 242 273 303 334 364 29
//30  030     090 121 151 182 212 243 274 304 335 365 30
//31  031     091     152     213 244     305     366 31
#define    APP_VERSION_STR  _T("083.25")
//#define    APP_VERSION_STR  _T("1.1.357.24")
//wxDateTime::GetJulianDayNumber();
//myVersion.Printf( wxT("1.1.%d.24"), wxDateTime::GetJulianDayNumber());


class WxBaseballApp : public wxApp
{
    public:
        virtual bool OnInit();
        int OnExit();

//		wxString myAppVersion;

    // ---------------------------------------------------------------------------
    // global variables
    // ---------------------------------------------------------------------------
    WxBaseballFrame* pWxBaseballFrame;
//    wxFrame* pWxBaseballFrame;

    DBRoutines* pDBRoutines;
    FileRoutines* pFileRoutines;

    // Global print data, to remember settings during the session
    wxPrintData* g_printData;

    // Global page setup data
    wxPageSetupData* g_pageSetupData;

    // Global Data Directory without the trailing slash
    wxString g_DataDirectory;

    // Global Options Directory and file name
    //    OptionsFile contains Dir nad filename
    // Access - wxGetApp().g_OptionsDir
    //          wxGetApp().g_OptionsFile
    wxString g_OptionsDir;
    wxString g_OptionsFile;

    //Global Option Variables
    wxString g_DataDir;

    wxString g_HTML_BG;
    wxString g_HTML_Index;
    wxString g_HTML_TextColor;
    wxString g_HTML_BGColor;
    wxString g_HTML_LinkColor;
    wxString g_HTML_VLinkColor;
};

DECLARE_APP(WxBaseballApp);

#endif // WXBASEBALLAPP_H
