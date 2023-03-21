#include "Driver.h"
#include "ffs.h"




#define CORE_STATUS1_REG		0x1006
#define WIN_OV_BIT			BIT(0)
#define WIN_UV_BIT			BIT(1)
#define EN_PIN_OUT_BIT			BIT(2)
#define LCM_AUTO_BIT			BIT(3)
#define LCM_PIN_BIT			BIT(4)
#define ILIM_BIT			BIT(5)
#define TEMP_ALARM_BIT			BIT(6)
#define VPH_OV_SOFT_BIT			BIT(7)

#define CORE_STATUS2_REG		0x1007
#define SWITCHER_HOLD_OFF_BIT		BIT(0)
#define VPH_OV_HARD_BIT			BIT(1)
#define TSD_BIT				BIT(2)
#define IREV_BIT			BIT(3)
#define IOC_BIT				BIT(4)
#define VIN_UV_BIT			BIT(5)
#define VIN_OV_BIT			BIT(6)
#define EN_PIN_OUT2_BIT			BIT(7)

#define CORE_STATUS3_REG		0x1008
#define EN_SL_BIT			BIT(0)
#define IIN_REF_SS_DONE_BIT		BIT(1)
#define FLYCAP_SS_DONE_BIT		BIT(2)
#define SL_DETECTED_BIT			BIT(3)

#define CORE_INT_RT_STS_REG		0x1010
#define SWITCHER_OFF_WINDOW_STS_BIT	BIT(0)
#define SWITCHER_OFF_FAULT_STS_BIT	BIT(1)
#define TSD_STS_BIT			BIT(2)
#define IREV_STS_BIT			BIT(3)
#define VPH_OV_HARD_STS_BIT		BIT(4)
#define VPH_OV_SOFT_STS_BIT		BIT(5)
#define ILIM_STS_BIT			BIT(6)
#define TEMP_ALARM_STS_BIT		BIT(7)

#define CORE_CONTROL1_REG		0x1020
#define CMD_EN_SWITCHER_BIT		BIT(0)
#define CMD_EN_SL_BIT			BIT(1)

#define CORE_FTRIM_ILIM_REG		0x1030
#define CFG_ILIM_MASK			GENMASK(4, 0)

#define CORE_FTRIM_LVL_REG		0x1033
#define CFG_WIN_HI_MASK			GENMASK(3, 2)
#define WIN_OV_LVL_1000MV		0x08

#define CORE_FTRIM_MISC_REG		0x1034
#define TR_WIN_1P5X_BIT			BIT(0)
#define WINDOW_DETECTION_DELTA_X1P0	0
#define WINDOW_DETECTION_DELTA_X1P5	1

#define CORE_ATEST1_SEL_REG		0x10E2
#define ATEST1_OUTPUT_ENABLE_BIT	BIT(7)
#define ATEST1_SEL_MASK			GENMASK(6, 0)
#define ISNS_INT_VAL			0x09

#define BATT_PROFILE_VOTER	"BATT_PROFILE_VOTER"
#define CP_VOTER		"CP_VOTER"
#define USER_VOTER		"USER_VOTER"
#define ILIM_VOTER		"ILIM_VOTER"
#define FCC_VOTER		"FCC_VOTER"
#define ICL_VOTER		"ICL_VOTER"
#define ICL_CHANGE_VOTER	"ICL_CHANGE_VOTER"
#define TAPER_END_VOTER		"TAPER_END_VOTER"
#define WIRELESS_VOTER		"WIRELESS_VOTER"
#define SRC_VOTER		"SRC_VOTER"
#define SWITCHER_TOGGLE_VOTER	"SWITCHER_TOGGLE_VOTER"
#define SOC_LEVEL_VOTER		"SOC_LEVEL_VOTER"



static void smb1390_read(PDEVICE_CONTEXT regmap, u16 addr, u8* val)
{
	u8 cmd[2];
	cmd[1] = addr & 0xff;
	cmd[0] = (addr >> 8) & 0xff;

	SpbDeviceWriteRead(regmap, cmd, &val, 2, 1);
}

static void smb1390_update_bits(PDEVICE_CONTEXT regmap,u16 reg, u8 mask, u8 val ) {
	u8 temp_val, data;
	smb1390_read(regmap, reg, &data);

	temp_val = data & ~mask;
	temp_val |= val & mask;

	if (data == temp_val)
	{
		return;
	}

	u8 cmd[3];
	cmd[1] = reg & 0xff;
	cmd[0] = (reg >> 8) & 0xff;
	cmd[2] = temp_val;
	SpbDeviceWrite(regmap, cmd, 3);
}
static void smb1390_masked_write(PDEVICE_CONTEXT regmap, u16 reg, u8 mask,
	u8 val)
{
	DbgPrint("Writing 0x%02x to 0x%04x with mask 0x%02x\n", val, reg, mask);
	smb1390_update_bits(regmap, reg, mask, val);
}

void  charger_init(struct device_node* dev_node) {
	u8 val;
	PDEVICE_CONTEXT chip = dev_node;
	smb1390_masked_write(chip, CORE_FTRIM_LVL_REG,
		CFG_WIN_HI_MASK, WIN_OV_LVL_1000MV);

	smb1390_masked_write(chip, CORE_FTRIM_MISC_REG,
		TR_WIN_1P5X_BIT, WINDOW_DETECTION_DELTA_X1P0);

	smb1390_read(chip, 0x1032, &val);
	DbgPrint("default smb1390 400K 0x1032_REG: 0x%x\n", val);

	smb1390_masked_write(chip, 0x1032, 0x0F, 0x07);
	smb1390_read(chip, 0x1032, &val);
	DbgPrint("modify smb1390 800K 0x1032_REG: 0x%x\n", val);
}