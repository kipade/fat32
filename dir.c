void get_fat_info(unsigned int fat32_sec)
{
        unsigned char *ptr = read_sec(fat32_sec);
        memcpy((void*)&fat, ptr, sizeof(fat));
        printf("%s\n", fat.BS_OEMName);
        printf("size: %d\n", fat.BPB_BytsPerSec);
}

/* 傳回指向 cluster 的 pointer
 * 要做的事情:
 * 1. 算出"相對於"該 partition 的 sector
 * 2. 呼叫 sb_bread
 * 3. 傳回 bh->b_data
 */
void *fat_read_clus(unsigned int cluster)
{
}

unsigned char *entry_buf = NULL;
int __fat_get_entry_slow(struct buffer_head **bh, struct inode **inod, struct dir_entry **de)
{
	bh = sb_bread();
	entry_buf = fat_read_clus(2);
	*de = (struct dir_entry*)entry_buf;
}

int fat_get_entry(struct buffer_head **bh, struct dir_entry **de)
{
	if (*bh && *de && (*de - bh->b_data < 4096)) {
		(*de)++;
		return 0;
	}
	return __fat_get_entry_slow(bh, de);
}

int fat_parse_long(struct dir_entry **de)
{
	char filename[260];
	unsigned char slot;
	struct dir_long_entry *dle = (struct dir_long_entry*)*de;
	if (!(dle->id & 0x40)) {
		printf("Parse error!");
		return -1;
	}

	slot = dle->id & ~0x40;
	/* 開始 parse 直到 id = 1 */
	while (1) {
		--slot;
		namecpy(filename + slot * 13, dle->name0_4, 5);
		namecpy(filename + slot * 13 + 5, dle->name5_10, 6);
		namecpy(filename + slot * 13 + 11, dle->name11_12, 2);

		if (slot == 0)
			break;
		fat_get_entry(de);
		dle = (struct dir_long_entry*)*de;
	}
	printf("Found %s\n", filename);
	return 0;
}

int vfat_find(char *fname)
{
	struct dir_entry *de = NULL;

	while (1) {
		fat_get_entry(&de);
		if (de->name[0] == 0xe5)
			continue;
		if (de->name[0] == 0x0)
			break;
		if (de->attr == 0x0f) {
			fat_parse_long(&de);
		}
	}
}