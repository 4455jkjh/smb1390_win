#include "Driver.h"


#define REVID_REVISION1	0x0
#define REVID_REVISION2	0x1
#define REVID_REVISION3	0x2
#define REVID_REVISION4	0x3
#define REVID_TYPE	0x4
#define REVID_SUBTYPE	0x5
#define REVID_STATUS1	0x8
#define REVID_SPARE_0	0x60
#define REVID_TP_REV	0xf1
#define REVID_FAB_ID	0xf2

#define QPNP_REVID_DEV_NAME "qcom,qpnp-revid"

static const char* const pmic_names[] = {
	[0] = "Unknown PMIC",
	[PM8941_SUBTYPE] = "PM8941",
	[PM8841_SUBTYPE] = "PM8841",
	[PM8019_SUBTYPE] = "PM8019",
	[PM8226_SUBTYPE] = "PM8226",
	[PM8110_SUBTYPE] = "PM8110",
	[PMA8084_SUBTYPE] = "PMA8084",
	[PMI8962_SUBTYPE] = "PMI8962",
	[PMD9635_SUBTYPE] = "PMD9635",
	[PM8994_SUBTYPE] = "PM8994",
	[PMI8994_SUBTYPE] = "PMI8994",
	[PM8916_SUBTYPE] = "PM8916",
	[PM8004_SUBTYPE] = "PM8004",
	[PM8909_SUBTYPE] = "PM8909",
	[PM2433_SUBTYPE] = "PM2433",
	[PMD9655_SUBTYPE] = "PMD9655",
	[PM8950_SUBTYPE] = "PM8950",
	[PMI8950_SUBTYPE] = "PMI8950",
	[PMK8001_SUBTYPE] = "PMK8001",
	[PMI8996_SUBTYPE] = "PMI8996",
	[PM8998_SUBTYPE] = "PM8998",
	[PMI8998_SUBTYPE] = "PMI8998",
	[PM8005_SUBTYPE] = "PM8005",
	[PM8937_SUBTYPE] = "PM8937",
	[PM660L_SUBTYPE] = "PM660L",
	[PM660_SUBTYPE] = "PM660",
	[PMI632_SUBTYPE] = "PMI632",
	[PMI8937_SUBTYPE] = "PMI8937",
	[PM8150_SUBTYPE] = "PM8150",
	[PM8150B_SUBTYPE] = "PM8150B",
	[PM8150L_SUBTYPE] = "PM8150L",
	[PM6150_SUBTYPE] = "PM6150",
	[PM8150A_SUBTYPE] = "PM8150A",
	[PME9205_SUBTYPE] = "PME9205",
	[PM6125_SUBTYPE] = "PM6125",
};


#define PM8941_PERIPHERAL_SUBTYPE	0x01
#define PM8226_PERIPHERAL_SUBTYPE	0x04
#define PMD9655_PERIPHERAL_SUBTYPE	0x0F
#define PMI8950_PERIPHERAL_SUBTYPE	0x11
#define PMI8937_PERIPHERAL_SUBTYPE	0x37
static u16 revid_base = 0x100;

static u8 qpnp_read_byte(PDEVICE_CONTEXT regmap, u16 addr)
{
	u8 val = 0;
	u8 cmd[2];
	cmd[1] = addr & 0xff;
	cmd[0] = (addr >> 8) & 0xff;

	SpbDeviceWriteRead(regmap, cmd, &val, 2, 1);
	return val;
}


#define PMIC_PERIPHERAL_TYPE		0x51
#define PMIC_STRING_MAXLENGTH		80
NTSTATUS revid_init(struct device_node* dev_node) {
	u8 rev1, rev2, rev3, rev4, pmic_type, pmic_subtype, pmic_status;
	u8 option1, option2, option3, option4, spare0;
	int fab_id, tp_rev;
	struct pmic_revid_data data = dev_node->revid_data;
	PDEVICE_CONTEXT regmap = dev_node;
	u16 base = revid_base;

	pmic_type = qpnp_read_byte(regmap, base + REVID_TYPE);
	if (pmic_type != PMIC_PERIPHERAL_TYPE) {
		DbgPrint("Invalid REVID peripheral type: %02X\n", pmic_type);
		return STATUS_POWER_STATE_INVALID;
	}

	rev1 = qpnp_read_byte(regmap, base + REVID_REVISION1);
	rev2 = qpnp_read_byte(regmap, base + REVID_REVISION2);
	rev3 = qpnp_read_byte(regmap, base + REVID_REVISION3);
	rev4 = qpnp_read_byte(regmap, base + REVID_REVISION4);

	pmic_subtype = qpnp_read_byte(regmap, base + REVID_SUBTYPE);
	if (pmic_subtype != PMD9655_PERIPHERAL_SUBTYPE)
		pmic_status = qpnp_read_byte(regmap, base + REVID_STATUS1);
	else
		pmic_status = 0;

	/* special case for PMI8937 */
	if (pmic_subtype == PMI8950_PERIPHERAL_SUBTYPE) {
		/* read spare register */
		spare0 = qpnp_read_byte(regmap, base + REVID_SPARE_0);
		if (spare0)
			pmic_subtype = PMI8937_PERIPHERAL_SUBTYPE;
	}
	fab_id = -22;
	tp_rev = -22;

	data.rev1 = rev1;
	data.rev2 = rev2;
	data.rev3 = rev3;
	data.rev4 = rev4;
	data.pmic_subtype = pmic_subtype;
	data.pmic_type = pmic_type;
	data.fab_id = fab_id;
	data.tp_rev = tp_rev;


	if (pmic_subtype < sizeof(pmic_names))
		data.pmic_name = pmic_names[pmic_subtype];
	else
		data.pmic_name = pmic_names[0];

	option1 = pmic_status & 0x3;
	option2 = (pmic_status >> 2) & 0x3;
	option3 = (pmic_status >> 4) & 0x3;
	option4 = (pmic_status >> 6) & 0x3;

	DbgPrint("%s options: %d, %d, %d, %d\n",
		"QPNP_REVID", option1, option2, option3, option4);
	return STATUS_SUCCESS;
}