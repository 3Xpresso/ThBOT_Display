// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.
//Technical support:goodtft@163.com

#include "src/Adafruit-GFX-Library-master/Adafruit_GFX.h"    // Core graphics library
#include "src/Adafruit_TFTLCD/Adafruit_TFTLCD.h" // Hardware-specific library

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define BLACK   (uint16_t)0x0000
#define BLUE    (uint16_t)0x001F
#define RED     (uint16_t)0xF800
#define GREEN   (uint16_t)0x07E0
#define CYAN    (uint16_t)0x07FF
#define MAGENTA (uint16_t)0xF81F
#define YELLOW  (uint16_t)0xFFE0
#define WHITE   (uint16_t)0xFFFF
#define JJCOLOR (uint16_t)0x1CB6

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;
#define TFTWIDTH   320
#define TFTHEIGHT  480


// 5-Psition Switch
#define S5P_SWT_UP  12
#define S5P_SWT_DN  10
#define S5P_SWT_RT  11
#define S5P_SWT_LT  13
#define S5P_SWT_CTR 19  //A5

enum {
  KEY_NONE,
  KEY_UP,
  KEY_DOWN,
  KEY_RIGHT,
  KEY_LEFT,
  KEY_ENTER
};

static unsigned char uc_StickState = 0;
int ValUp = 1;
int ValDown = 1;
int ValLeft = 1;
int ValRigh = 1;
int ValCtr = 1;

enum {
  MODE_IDLE,
  MODE_COMPET,
  MODE_QUALIF,
  MODE_CALIBR,
  MODE_TESTS
};

enum {
  STATE_IDLE,
  STATE_GET_PWR_STATE,
  STATE_ENC_READ,
  STATE_MOVE_FORWARD,
  STATE_MOVE_BACKWARD,
  STATE_TURN_RIGTH,
  STATE_TURN_LEFT,
};

enum {
  PARAM_NONE,
  PARAM_PWM,

  /***********/
  PARAM_MAX
};

#define MAX_ENTRY               14
#define MAX_HIERARCHY_LEVEL     4
#define YPOS_HEADER_OFFSET      40
#define YPOS_ENTRY_OFFSET       20

#define XPOS_TEXT_OFFSET       4
#define YPOS_TEXT_OFFSET       4

#define X_PARAM_SIZE           24
#define Y_PARAM_SIZE           38

typedef struct {
  int16_t X;
  int16_t Y;
  char* HeaderName;
  uint16_t BckGndColor;
  uint16_t TextColor;
} t_HeaderMenu;

typedef void (*fn_ptr) (void);

typedef struct {
  int16_t X;
  int16_t Y;
  const char* FieldName;
  uint16_t BckGndColor;
  uint16_t TextColor;
  fn_ptr action;  // routine to call when selected
  const struct Menu *NextMenu;
}t_MenuEntry;

// http://www.avrfreaks.net/forum/progmem-using-structs-and-arrays
typedef struct Menu
{
  t_MenuEntry MenuEntry[MAX_ENTRY];
  t_HeaderMenu st_HeaderMenu;
} t_Menu;


const char EmptyText[] PROGMEM = "";

// MENU TESTS MOTEURS
const char TestsMoteursMenuText[] PROGMEM = "TESTS MOTEURS";
const char PWMPercentText[] PROGMEM = "PWM %";
const char MarcheAvantText[] PROGMEM = "Marche avant";
const char MarcheArriereText[] PROGMEM = "Marche arriere";
const char RotationDroiteText[] PROGMEM = "Rotation a droite";
const char RotationGaucheText[] PROGMEM = "Rotation a gauche";

void ActTestPwmPercentage(void);
void ActTestMarcheAvant(void);
void ActTestMarcheArriere(void);
void ActTestRotationDoite(void);
void ActTestRotationGauche(void);

const t_Menu st_TestsMoteursMenu PROGMEM = {
  .MenuEntry = {
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*0,  PWMPercentText,     BLUE,    WHITE, ActTestPwmPercentage,  NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*1,  MarcheAvantText,    JJCOLOR, WHITE, ActTestMarcheAvant,    NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*2,  MarcheArriereText,  BLUE,    WHITE, ActTestMarcheArriere,  NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*3,  RotationDroiteText, JJCOLOR, WHITE, ActTestRotationDoite,  NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*4,  RotationGaucheText, BLUE,    WHITE, ActTestRotationGauche, NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*5,  EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*6,  EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*7,  EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*8,  EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*9,  EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*10, EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*11, EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*12, EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*13, EmptyText,          BLACK,   GREEN, NULL,NULL},
  },
  .st_HeaderMenu = {
    XPOS_TEXT_OFFSET*2, YPOS_TEXT_OFFSET*2, (char*)TestsMoteursMenuText, YELLOW, RED
  },
};

// MENU TESTS
const char TestsMenuText[] PROGMEM = "TESTS";
const char ArretUrgenceText[] PROGMEM = "Arret Urgence";
const char MoteursText[] PROGMEM = "Moteurs";
const char EncodeursText[] PROGMEM = "Encodeurs";

void ActTestArretUrgence(void);
void ActTestEncodeurs(void);

const t_Menu st_TestsMenu PROGMEM = {
  .MenuEntry = {
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*0,  ArretUrgenceText,   BLUE,    WHITE, ActTestArretUrgence,   NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*1,  MoteursText,        JJCOLOR, WHITE, NULL,                  &st_TestsMoteursMenu},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*2,  EncodeursText,      BLUE,    WHITE, ActTestEncodeurs,      NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*3,  EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*4,  EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*5,  EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*6,  EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*7,  EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*8,  EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*9,  EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*10, EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*11, EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*12, EmptyText,          BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*13, EmptyText,          BLACK,   GREEN, NULL,NULL},
  },
  .st_HeaderMenu = {
    XPOS_TEXT_OFFSET*2, YPOS_TEXT_OFFSET*2, (char*)TestsMenuText, YELLOW, RED
  },
};

// MENU : MODE
const char SelectModeText[] PROGMEM = "Select Mode";
const char CompetitionText[] PROGMEM = "Competition";
const char QualificationText[] PROGMEM = "Qualification";
const char CalibrationText[] PROGMEM = "Calibration";
const char TestsText[] PROGMEM = "Tests";

void SendModeIdle(void);
void SendModeCompet(void);
void SendModeQualif(void);
void SendModeCalibr(void);
void SendModeTests(void);

const t_Menu st_MainMenu PROGMEM = {
  .MenuEntry = {
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*0,  CompetitionText,   BLUE,    WHITE, SendModeCompet,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*1,  QualificationText, JJCOLOR, WHITE, SendModeQualif,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*2,  CalibrationText,   BLUE,    WHITE, SendModeCalibr,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*3,  TestsText,         JJCOLOR, WHITE, SendModeTests, &st_TestsMenu},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*4,  EmptyText,         BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*5,  EmptyText,         BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*6,  EmptyText,         BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*7,  EmptyText,         BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*8,  EmptyText,         BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*9,  EmptyText,         BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*10, EmptyText,         BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*11, EmptyText,         BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*12, EmptyText,         BLACK,   GREEN, NULL,NULL},
    { 0, YPOS_HEADER_OFFSET+YPOS_ENTRY_OFFSET*13, EmptyText,         BLACK,   GREEN, NULL,NULL},
  },
  .st_HeaderMenu = {
    XPOS_TEXT_OFFSET*2, YPOS_TEXT_OFFSET*2, (char*)SelectModeText, YELLOW, RED
  },
};

typedef struct MenuHierarchy {
  const t_Menu * pst_MenuLvl;
  uint16_t MenuEntryIndex;
} t_MenuHierarchy;

struct st_MenuHierarchy {
  int16_t HierarchyIndex = 0;
  t_MenuHierarchy hierarchy[MAX_HIERARCHY_LEVEL] = {
    { .pst_MenuLvl = (t_Menu*)&st_MainMenu, .MenuEntryIndex = 0 },
    { .pst_MenuLvl = (t_Menu*)NULL, .MenuEntryIndex = 0 },
    { .pst_MenuLvl = (t_Menu*)NULL, .MenuEntryIndex = 0 },
    { .pst_MenuLvl = (t_Menu*)NULL, .MenuEntryIndex = 0 }
  };
} st_MenuHierarchy;

void Printf(const char *p_pcFormat,...)
{
  char l_acMessage[128];
  va_list l_tArgp;
  int iNbBytes;
  int iIndex;

  va_start(l_tArgp, p_pcFormat);   
  iNbBytes = vsnprintf(l_acMessage, 128, p_pcFormat, l_tArgp);
  va_end(l_tArgp);
  
  for (iIndex=0; iIndex < iNbBytes; iIndex ++)
  {
    Serial.write(l_acMessage[iIndex]);
  }
  Serial.flush();
}

void TftPrintf(const char *p_pcFormat,...)
{
  char l_acMessage[128];
  va_list l_tArgp;
  int iNbBytes;
  int iIndex;

  va_start(l_tArgp, p_pcFormat);   
  iNbBytes = vsnprintf(l_acMessage, 128, p_pcFormat, l_tArgp);
  va_end(l_tArgp);
  
  /*for (iIndex=0; iIndex < iNbBytes; iIndex ++)
  {
    tft.write(l_acMessage[iIndex]);
  }*/
  tft.println(l_acMessage);
}

unsigned char CheckJoystick(void)
{
  unsigned char uc_RetVal = KEY_NONE;
  int TmpVal;

  TmpVal = digitalRead(S5P_SWT_UP);
  if ((TmpVal != ValUp) && (TmpVal == 0))
  {
    //Printf("Up = %i (%i)\n", ValUp, TmpVal);
    uc_RetVal = KEY_UP;
  }
  ValUp = TmpVal;
  
  TmpVal = digitalRead(S5P_SWT_DN);
  if ((TmpVal != ValDown) && (TmpVal == 0))
  {
    //Printf("DN = %i (%i)\n", ValDown, TmpVal);
    uc_RetVal = KEY_DOWN;
  }
  ValDown = TmpVal;
  
  TmpVal = digitalRead(S5P_SWT_RT);
  if ((TmpVal != ValRigh) && (TmpVal == 0))
  {
    //Printf("RT = %i (%i)\n", ValRigh, TmpVal);
    uc_RetVal = KEY_RIGHT;
  }
  ValRigh = TmpVal;

  TmpVal = digitalRead(S5P_SWT_LT);
  if ((TmpVal != ValLeft) && (TmpVal == 0))
  {
    //Printf("LT = %i (%i)\n", ValLeft, TmpVal);
    uc_RetVal = KEY_LEFT;
  }
  ValLeft = TmpVal;

  TmpVal = digitalRead(S5P_SWT_CTR);
  if ((TmpVal != ValCtr) && (TmpVal == 0))
  {
    //Printf("CTR = %i (%i)\n", ValCtr, TmpVal);
    uc_RetVal = KEY_ENTER;
  }
  ValCtr = TmpVal;

  return uc_RetVal;
}

struct st_CurrentEntry {
  int YPos;
  uint16_t BckGndColor;
} st_CurrentEntry;

uint16_t GetEntry(int Step)
{
  char EntryName[32];
  const t_Menu * pst_currentMenu = st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].pst_MenuLvl;
  int Index = st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].MenuEntryIndex;
  
  do
  {
    Index+=Step;
    if (Index >= MAX_ENTRY)
      Index = 0;

    if (Index < 0)
      Index = MAX_ENTRY-1;

    strcpy_P(EntryName, (char*)pgm_read_word(&pst_currentMenu->MenuEntry[Index].FieldName));
  }while(EntryName[0] == '\0');

  return Index;
}

void UpdateSelectENtry(int8_t Position)
{
  uint16_t NextEntryIndex;
  const t_Menu * pst_currentMenu = st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].pst_MenuLvl;
  
  switch (Position)
  {
    case -1:
    {
      // Clean current rectangle
      tft.drawRect(0, st_CurrentEntry.YPos, TFTHEIGHT, YPOS_ENTRY_OFFSET, st_CurrentEntry.BckGndColor);

      NextEntryIndex              = GetEntry(-1);
      st_CurrentEntry.YPos        = YPOS_HEADER_OFFSET + YPOS_ENTRY_OFFSET * NextEntryIndex;
      st_CurrentEntry.BckGndColor = pgm_read_word(&pst_currentMenu->MenuEntry[NextEntryIndex].BckGndColor);
    }break;
    case 0:
    {
      NextEntryIndex              = st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].MenuEntryIndex;
      st_CurrentEntry.YPos        = YPOS_HEADER_OFFSET + YPOS_ENTRY_OFFSET * NextEntryIndex;
      st_CurrentEntry.BckGndColor = pgm_read_word(&pst_currentMenu->MenuEntry[NextEntryIndex].BckGndColor);
    }break;
    case 1:
    {
      // Clean current rectangle
      tft.drawRect(0, st_CurrentEntry.YPos, TFTHEIGHT, YPOS_ENTRY_OFFSET, st_CurrentEntry.BckGndColor);

      NextEntryIndex              = GetEntry(+1);
      st_CurrentEntry.YPos        = YPOS_HEADER_OFFSET + YPOS_ENTRY_OFFSET * NextEntryIndex;
      st_CurrentEntry.BckGndColor = pgm_read_word(&pst_currentMenu->MenuEntry[NextEntryIndex].BckGndColor);
    }break;
  }
  
  // Draw new one
  tft.drawRect(0, st_CurrentEntry.YPos, TFTHEIGHT, YPOS_ENTRY_OFFSET, WHITE);

  st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].MenuEntryIndex = NextEntryIndex;
}

void DisplayMenu()
{
    char HeaderName[32];
    const t_Menu * pst_currentMenu = st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].pst_MenuLvl;
    uint16_t BckGndColor = pgm_read_word(&pst_currentMenu->st_HeaderMenu.BckGndColor);
    uint16_t TextColor = pgm_read_word(&pst_currentMenu->st_HeaderMenu.TextColor);
    int XposHeader = pgm_read_word(&pst_currentMenu->st_HeaderMenu.X);
    int YposHeader = pgm_read_word(&pst_currentMenu->st_HeaderMenu.Y);;

    strcpy_P(HeaderName, (char*)pgm_read_word(&pst_currentMenu->st_HeaderMenu.HeaderName));
      
    tft.fillScreen(BLACK);
    tft.fillRect(0, 0, TFTHEIGHT, YPOS_HEADER_OFFSET, BckGndColor);
    tft.drawRect(0, 0, TFTHEIGHT, YPOS_HEADER_OFFSET, YELLOW);
    
    tft.setCursor(XposHeader, YposHeader);
    tft.setTextColor(TextColor);
    tft.setTextSize(3);
    tft.println(HeaderName);

    for (int Index = 0; Index < MAX_ENTRY; Index++)
    {
      char FieldName[32];
      int XposText = pgm_read_word(&pst_currentMenu->MenuEntry[Index].X)+XPOS_TEXT_OFFSET;
      int YposText = pgm_read_word(&pst_currentMenu->MenuEntry[Index].Y)+YPOS_TEXT_OFFSET;
      BckGndColor  = pgm_read_word(&pst_currentMenu->MenuEntry[Index].BckGndColor);
      TextColor    = pgm_read_word(&pst_currentMenu->MenuEntry[Index].TextColor);
      
      strcpy_P(FieldName, (char*)pgm_read_word(&pst_currentMenu->MenuEntry[Index].FieldName));

      tft.fillRect(0, YposText-YPOS_TEXT_OFFSET, TFTHEIGHT, YPOS_ENTRY_OFFSET, BckGndColor);
      tft.setTextColor(TextColor);
      tft.setTextSize(2);
      tft.setCursor(XposText, YposText);
      tft.println(FieldName);
    }
    st_CurrentEntry.YPos = YPOS_HEADER_OFFSET;

    UpdateSelectENtry(0);
}

void DisplayNextMenu()
{
  const t_Menu * pst_CurrentMenu = st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].pst_MenuLvl;;
  uint16_t CurrentMenuEntryIndex = st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].MenuEntryIndex;
  const t_Menu * pst_NextMenu = NULL;
  fn_ptr EntryAction;

  if ( st_MenuHierarchy.HierarchyIndex+1 < MAX_HIERARCHY_LEVEL)
  {
    pst_NextMenu = (const t_Menu *)pgm_read_word(&pst_CurrentMenu->MenuEntry[CurrentMenuEntryIndex].NextMenu);

    EntryAction = (fn_ptr)pgm_read_word(&pst_CurrentMenu->MenuEntry[st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].MenuEntryIndex].action);
    //Printf("EntryAction %p\n", EntryAction);
    if (EntryAction != 0)
      EntryAction();

    if (pst_NextMenu != 0)
    {
      st_MenuHierarchy.HierarchyIndex++;
      st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].pst_MenuLvl = pst_NextMenu;
      DisplayMenu();
    }
  }
}

void DisplayPreviousMenu()
{
  const t_Menu * pst_CurrentMenu = st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].pst_MenuLvl;;

  if ( st_MenuHierarchy.HierarchyIndex > 0)
  {
    st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].pst_MenuLvl    = NULL;
    st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].MenuEntryIndex = 0;

    st_MenuHierarchy.HierarchyIndex--;
    DisplayMenu();

    if (st_MenuHierarchy.hierarchy[st_MenuHierarchy.HierarchyIndex].pst_MenuLvl == &st_MainMenu)
    {
      SendModeIdle();
    }
  }
}

void SendModeIdle(void)
{
  Printf("CMD:%2.2d:%2.2d\n", MODE_IDLE, STATE_IDLE);
}

void SendModeCompet(void)
{
  Printf("CMD:%2.2d:%2.2d\n", MODE_COMPET, STATE_IDLE);
}

void SendModeQualif(void)
{
  Printf("CMD:%2.2d:%2.2d\n", MODE_QUALIF, STATE_IDLE);
}

void SendModeCalibr(void)
{
  Printf("CMD:%2.2d:%2.2d\n", MODE_CALIBR, STATE_IDLE);
}

void SendModeTests(void)
{
  Printf("CMD:%2.2d:%2.2d\n", MODE_TESTS, STATE_IDLE);
}

void DisplayTestArretUrgence(char * RespBuff)
{
  /* RSP:04:01:ON 
   * RSP:04:01:OFF
   */
  RespBuff[3] = '\0';
  RespBuff[6] = '\0';
  RespBuff[9] = '\0';
  RespBuff[13] = '\0';
  RespBuff[15] = '\0';

  tft.setCursor(0, 50);
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  TftPrintf("Motor power : %s\n", &RespBuff[10]);
}

void DisplayTestEncodeurs(char * RespBuff)
{
  /* RSP:04:02:0000065537:+:2000001024:-
   * RSP:04:02:0000000000:+:0000000000:-
   */
  RespBuff[3] = '\0';
  RespBuff[6] = '\0';
  RespBuff[9] = '\0';
  RespBuff[20] = '\0';
  RespBuff[22] = '\0';
  RespBuff[33] = '\0';
  RespBuff[35] = '\0';

  tft.fillScreen(BLACK);
  tft.setCursor(0, 50);
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  TftPrintf("Left  : %s%s\n", &RespBuff[21], &RespBuff[10]);
  TftPrintf("Right : %s%s\n", &RespBuff[34], &RespBuff[23]);
}

static int RespBuffIndex=0;

int WaitResponse(char * RespBuff, int RespLen)
{
  int incomingByte = 0;
  int RespBuffOffset = 0;
  
  /* Manage response */
  incomingByte = Serial.available();
  if (incomingByte > 0) {
    Serial.readBytes(&RespBuff[RespBuffIndex], incomingByte);
    RespBuffIndex += incomingByte;
    
    if (RespBuffIndex >= RespLen-1)
    {
      /* Search Response */
      for (int Index=0; Index<RespBuffIndex; Index++)
      {
        if (   (RespBuff[Index] == 'R')
            && (RespBuff[Index+1] == 'S')
            && (RespBuff[Index+2] == 'P')
           )
        {
          RespBuffOffset = Index;

          if (RespBuffOffset > 0)
          {
            for (int Index=0; Index< RespBuffIndex; Index++)
            {
              RespBuff[Index] = RespBuff[RespBuffOffset+Index];
            }
          }
          RespBuffIndex -= RespBuffOffset;
        }
      }
    }
  }

  if (RespBuffIndex >= (RespLen-1))
    return 1;
  else
    return 0;
}

void ActTestArretUrgence(void)
{
  char BufferResp[64];
  int  RespAvailable;

  memset(BufferResp, 0, 64);
  RespBuffIndex = 0;
  
  Printf("CMD:%2.2d:%2.2d\n", MODE_TESTS, STATE_GET_PWR_STATE);

  tft.fillScreen(BLACK);
  DisplayTestArretUrgence(BufferResp);

  do
  {
    RespAvailable = WaitResponse(BufferResp, 14);

    uc_StickState = CheckJoystick();
  }while((uc_StickState != KEY_LEFT) && (RespAvailable == 0));

  DisplayTestArretUrgence(BufferResp);
  delay(1000);
  SendModeTests();
  DisplayMenu();
}

void ActTestEncodeurs(void)
{
  char BufferResp[64];
  int  RespAvailable;
  uint16_t u16_SleepLoop = 1;

  memset(BufferResp, 0, 64);
  RespBuffIndex = 0;
  
  tft.fillScreen(BLACK);
  DisplayTestEncodeurs(BufferResp);

  do
  {
    if (u16_SleepLoop > 0)
    {
      u16_SleepLoop--;
      if (u16_SleepLoop==0)
        Printf("CMD:%2.2d:%2.2d\n", MODE_TESTS, STATE_ENC_READ);
      delay(100);
    }
    else
    {
      RespAvailable = WaitResponse(BufferResp, 36);
    
      if (RespAvailable > 0)
      {
        DisplayTestEncodeurs(BufferResp);

        memset(BufferResp, 0, 64);
        RespBuffIndex = 0;

        /* Wait 2s before requesting a new read */
        u16_SleepLoop = 20;
      }
    }
    uc_StickState = CheckJoystick();
  }while(uc_StickState != KEY_LEFT);

  SendModeTests();
  DisplayMenu();
}

void ManageParamField(char * ParamBuff, int ParamLen, int XOffset, int YOffset)
{
  int ParamIndex = 0;
  int XPos = XOffset-3;

  tft.setCursor(XOffset, YOffset);
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  TftPrintf("%s\n", ParamBuff);
  
  do
  {
    if (uc_StickState > KEY_NONE)
    {
      switch (uc_StickState)
      {
        case KEY_UP:
        {
          if (ParamBuff[ParamIndex] == '0')
            ParamBuff[ParamIndex] = '9';
          else
            ParamBuff[ParamIndex] -= 1;
        }break;
        case KEY_DOWN:
        {
          if (ParamBuff[ParamIndex] == '9')
            ParamBuff[ParamIndex] = '0';
          else
            ParamBuff[ParamIndex] += 1;
        }break;
        case KEY_RIGHT:
        {
          if (ParamIndex < (ParamLen-1))
          {
            XPos += X_PARAM_SIZE;
            ParamIndex++;
          }
        }break;
        case KEY_LEFT:
        {
          if (ParamIndex >= 1)
          {
            XPos -= X_PARAM_SIZE;
            ParamIndex--;
          }
        }break;
        case KEY_ENTER:
        {
        }break;
      }
      tft.setCursor(XOffset, YOffset);
      tft.setTextColor(GREEN);
      tft.setTextSize(4);
      tft.fillRect(XOffset-3, YOffset-4, TFTHEIGHT-(XOffset-3), Y_PARAM_SIZE, BLACK);
      TftPrintf("%s\n", ParamBuff);
      tft.drawRect(XPos, YOffset-4, X_PARAM_SIZE, Y_PARAM_SIZE, WHITE);
    }
    uc_StickState = CheckJoystick();
  }while(uc_StickState != KEY_ENTER);
}

static char ParamPwmPercentage[8];

void ActTestPwmPercentage(void)
{
  int ParamIndex = 0;
  int XPos = 208;
  
  tft.fillScreen(BLACK);
  tft.setCursor(0, 50);
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  TftPrintf("PWM(%%) = ");

  ManageParamField(ParamPwmPercentage, 3, XPos, 50);

  Printf("PAR:%2.2d:%2.2d:%s\n", 3, PARAM_PWM, ParamPwmPercentage);
  DisplayMenu();
}
void ActTestMarcheAvant(void)
{
  Printf("CMD:%2.2d:%2.2d\n", MODE_TESTS, STATE_MOVE_FORWARD);
}

void ActTestMarcheArriere(void)
{
  Printf("CMD:%2.2d:%2.2d\n", MODE_TESTS, STATE_MOVE_BACKWARD);
}

void ActTestRotationDoite(void)
{
  Printf("CMD:%2.2d:%2.2d\n", MODE_TESTS, STATE_TURN_RIGTH);
}

void ActTestRotationGauche(void)
{
  Printf("CMD:%2.2d:%2.2d\n", MODE_TESTS, STATE_TURN_LEFT);
}

void setup(void) {
  Serial.begin(115200);
  Serial.println(F("TFT LCD test"));

#ifdef USE_ADAFRUIT_SHIELD_PINOUT
//  Serial.println(F("Using Adafruit 3.5\" TFT Arduino Shield Pinout"));
#else
//  Serial.println(F("Using Adafruit 3.5\" TFT Breakout Board Pinout"));
#endif

//  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier=0x9481;

  tft.begin(identifier);
  tft.setRotation(3);
  tft.fillScreen(BLACK);

  pinMode(S5P_SWT_UP, INPUT);
  pinMode(S5P_SWT_DN, INPUT);
  pinMode(S5P_SWT_RT, INPUT);
  pinMode(S5P_SWT_LT, INPUT);
  pinMode(S5P_SWT_CTR, INPUT);

  sprintf(ParamPwmPercentage, "%s", "010");

  st_MenuHierarchy.HierarchyIndex = 0;
  st_CurrentEntry.YPos            = 0;
  st_CurrentEntry.BckGndColor     = BLACK;
  
  DisplayMenu();
  SendModeIdle();
}

char Buffer[64];

void loop(void) {
  int incomingByte = 0;
  
  uc_StickState = CheckJoystick();

  incomingByte = Serial.available();
  if (incomingByte > 0) {
    Serial.readBytes(Buffer, incomingByte);
    Serial.flush();
    while (Serial.available()) {
      // get the new byte:
      char inChar = (char)Serial.read();
    }
  }

  if ( uc_StickState > KEY_NONE)
  {
    switch (uc_StickState)
    {
      case KEY_UP:
      {
        UpdateSelectENtry(-1);
      }break;
      case KEY_DOWN:
      {
        UpdateSelectENtry(1);
      }break;
      case KEY_RIGHT:
      {
      }break;
      case KEY_LEFT:
      {
        DisplayPreviousMenu();
      }break;
      case KEY_ENTER:
      {
        DisplayNextMenu();
      }break;
    }
  }
}


