/*
 * Copyright (c) 1995, 1996 Kungliga Tekniska H�gskolan (Royal Institute
 * of Technology, Stockholm, Sweden).
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the Kungliga Tekniska
 *      H�gskolan and its contributors.
 * 
 * 4. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
RCSID("$Id$");
#endif

#include "otp_locl.h"

struct e {
  char *s;
  unsigned n;
};

static struct e inv_std_dict[] = {
{"A", 0},
{"ABE", 1},
{"ABED", 571},
{"ABEL", 572},
{"ABET", 573},
{"ABLE", 574},
{"ABUT", 575},
{"ACE", 2},
{"ACHE", 576},
{"ACID", 577},
{"ACME", 578},
{"ACRE", 579},
{"ACT", 3},
{"ACTA", 580},
{"ACTS", 581},
{"AD", 4},
{"ADA", 5},
{"ADAM", 582},
{"ADD", 6},
{"ADDS", 583},
{"ADEN", 584},
{"AFAR", 585},
{"AFRO", 586},
{"AGEE", 587},
{"AGO", 7},
{"AHEM", 588},
{"AHOY", 589},
{"AID", 8},
{"AIDA", 590},
{"AIDE", 591},
{"AIDS", 592},
{"AIM", 9},
{"AIR", 10},
{"AIRY", 593},
{"AJAR", 594},
{"AKIN", 595},
{"ALAN", 596},
{"ALEC", 597},
{"ALGA", 598},
{"ALIA", 599},
{"ALL", 11},
{"ALLY", 600},
{"ALMA", 601},
{"ALOE", 602},
{"ALP", 12},
{"ALSO", 603},
{"ALTO", 604},
{"ALUM", 605},
{"ALVA", 606},
{"AM", 13},
{"AMEN", 607},
{"AMES", 608},
{"AMID", 609},
{"AMMO", 610},
{"AMOK", 611},
{"AMOS", 612},
{"AMRA", 613},
{"AMY", 14},
{"AN", 15},
{"ANA", 16},
{"AND", 17},
{"ANDY", 614},
{"ANEW", 615},
{"ANN", 18},
{"ANNA", 616},
{"ANNE", 617},
{"ANT", 19},
{"ANTE", 618},
{"ANTI", 619},
{"ANY", 20},
{"APE", 21},
{"APS", 22},
{"APT", 23},
{"AQUA", 620},
{"ARAB", 621},
{"ARC", 24},
{"ARCH", 622},
{"ARE", 25},
{"AREA", 623},
{"ARGO", 624},
{"ARID", 625},
{"ARK", 26},
{"ARM", 27},
{"ARMY", 626},
{"ART", 28},
{"ARTS", 627},
{"ARTY", 628},
{"AS", 29},
{"ASH", 30},
{"ASIA", 629},
{"ASK", 31},
{"ASKS", 630},
{"AT", 32},
{"ATE", 33},
{"ATOM", 631},
{"AUG", 34},
{"AUK", 35},
{"AUNT", 632},
{"AURA", 633},
{"AUTO", 634},
{"AVE", 36},
{"AVER", 635},
{"AVID", 636},
{"AVIS", 637},
{"AVON", 638},
{"AVOW", 639},
{"AWAY", 640},
{"AWE", 37},
{"AWK", 38},
{"AWL", 39},
{"AWN", 40},
{"AWRY", 641},
{"AX", 41},
{"AYE", 42},
{"BABE", 642},
{"BABY", 643},
{"BACH", 644},
{"BACK", 645},
{"BAD", 43},
{"BADE", 646},
{"BAG", 44},
{"BAH", 45},
{"BAIL", 647},
{"BAIT", 648},
{"BAKE", 649},
{"BALD", 650},
{"BALE", 651},
{"BALI", 652},
{"BALK", 653},
{"BALL", 654},
{"BALM", 655},
{"BAM", 46},
{"BAN", 47},
{"BAND", 656},
{"BANE", 657},
{"BANG", 658},
{"BANK", 659},
{"BAR", 48},
{"BARB", 660},
{"BARD", 661},
{"BARE", 662},
{"BARK", 663},
{"BARN", 664},
{"BARR", 665},
{"BASE", 666},
{"BASH", 667},
{"BASK", 668},
{"BASS", 669},
{"BAT", 49},
{"BATE", 670},
{"BATH", 671},
{"BAWD", 672},
{"BAWL", 673},
{"BAY", 50},
{"BE", 51},
{"BEAD", 674},
{"BEAK", 675},
{"BEAM", 676},
{"BEAN", 677},
{"BEAR", 678},
{"BEAT", 679},
{"BEAU", 680},
{"BECK", 681},
{"BED", 52},
{"BEE", 53},
{"BEEF", 682},
{"BEEN", 683},
{"BEER", 684},
{"BEET", 685},
{"BEG", 54},
{"BELA", 686},
{"BELL", 687},
{"BELT", 688},
{"BEN", 55},
{"BEND", 689},
{"BENT", 690},
{"BERG", 691},
{"BERN", 692},
{"BERT", 693},
{"BESS", 694},
{"BEST", 695},
{"BET", 56},
{"BETA", 696},
{"BETH", 697},
{"BEY", 57},
{"BHOY", 698},
{"BIAS", 699},
{"BIB", 58},
{"BID", 59},
{"BIDE", 700},
{"BIEN", 701},
{"BIG", 60},
{"BILE", 702},
{"BILK", 703},
{"BILL", 704},
{"BIN", 61},
{"BIND", 705},
{"BING", 706},
{"BIRD", 707},
{"BIT", 62},
{"BITE", 708},
{"BITS", 709},
{"BLAB", 710},
{"BLAT", 711},
{"BLED", 712},
{"BLEW", 713},
{"BLOB", 714},
{"BLOC", 715},
{"BLOT", 716},
{"BLOW", 717},
{"BLUE", 718},
{"BLUM", 719},
{"BLUR", 720},
{"BOAR", 721},
{"BOAT", 722},
{"BOB", 63},
{"BOCA", 723},
{"BOCK", 724},
{"BODE", 725},
{"BODY", 726},
{"BOG", 64},
{"BOGY", 727},
{"BOHR", 728},
{"BOIL", 729},
{"BOLD", 730},
{"BOLO", 731},
{"BOLT", 732},
{"BOMB", 733},
{"BON", 65},
{"BONA", 734},
{"BOND", 735},
{"BONE", 736},
{"BONG", 737},
{"BONN", 738},
{"BONY", 739},
{"BOO", 66},
{"BOOK", 740},
{"BOOM", 741},
{"BOON", 742},
{"BOOT", 743},
{"BOP", 67},
{"BORE", 744},
{"BORG", 745},
{"BORN", 746},
{"BOSE", 747},
{"BOSS", 748},
{"BOTH", 749},
{"BOUT", 750},
{"BOW", 68},
{"BOWL", 751},
{"BOY", 69},
{"BOYD", 752},
{"BRAD", 753},
{"BRAE", 754},
{"BRAG", 755},
{"BRAN", 756},
{"BRAY", 757},
{"BRED", 758},
{"BREW", 759},
{"BRIG", 760},
{"BRIM", 761},
{"BROW", 762},
{"BUB", 70},
{"BUCK", 763},
{"BUD", 71},
{"BUDD", 764},
{"BUFF", 765},
{"BUG", 72},
{"BULB", 766},
{"BULK", 767},
{"BULL", 768},
{"BUM", 73},
{"BUN", 74},
{"BUNK", 769},
{"BUNT", 770},
{"BUOY", 771},
{"BURG", 772},
{"BURL", 773},
{"BURN", 774},
{"BURR", 775},
{"BURT", 776},
{"BURY", 777},
{"BUS", 75},
{"BUSH", 778},
{"BUSS", 779},
{"BUST", 780},
{"BUSY", 781},
{"BUT", 76},
{"BUY", 77},
{"BY", 78},
{"BYE", 79},
{"BYTE", 782},
{"CAB", 80},
{"CADY", 783},
{"CAFE", 784},
{"CAGE", 785},
{"CAIN", 786},
{"CAKE", 787},
{"CAL", 81},
{"CALF", 788},
{"CALL", 789},
{"CALM", 790},
{"CAM", 82},
{"CAME", 791},
{"CAN", 83},
{"CANE", 792},
{"CANT", 793},
{"CAP", 84},
{"CAR", 85},
{"CARD", 794},
{"CARE", 795},
{"CARL", 796},
{"CARR", 797},
{"CART", 798},
{"CASE", 799},
{"CASH", 800},
{"CASK", 801},
{"CAST", 802},
{"CAT", 86},
{"CAVE", 803},
{"CAW", 87},
{"CEIL", 804},
{"CELL", 805},
{"CENT", 806},
{"CERN", 807},
{"CHAD", 808},
{"CHAR", 809},
{"CHAT", 810},
{"CHAW", 811},
{"CHEF", 812},
{"CHEN", 813},
{"CHEW", 814},
{"CHIC", 815},
{"CHIN", 816},
{"CHOU", 817},
{"CHOW", 818},
{"CHUB", 819},
{"CHUG", 820},
{"CHUM", 821},
{"CITE", 822},
{"CITY", 823},
{"CLAD", 824},
{"CLAM", 825},
{"CLAN", 826},
{"CLAW", 827},
{"CLAY", 828},
{"CLOD", 829},
{"CLOG", 830},
{"CLOT", 831},
{"CLUB", 832},
{"CLUE", 833},
{"COAL", 834},
{"COAT", 835},
{"COCA", 836},
{"COCK", 837},
{"COCO", 838},
{"COD", 88},
{"CODA", 839},
{"CODE", 840},
{"CODY", 841},
{"COED", 842},
{"COG", 89},
{"COIL", 843},
{"COIN", 844},
{"COKE", 845},
{"COL", 90},
{"COLA", 846},
{"COLD", 847},
{"COLT", 848},
{"COMA", 849},
{"COMB", 850},
{"COME", 851},
{"CON", 91},
{"COO", 92},
{"COOK", 852},
{"COOL", 853},
{"COON", 854},
{"COOT", 855},
{"COP", 93},
{"CORD", 856},
{"CORE", 857},
{"CORK", 858},
{"CORN", 859},
{"COST", 860},
{"COT", 94},
{"COVE", 861},
{"COW", 95},
{"COWL", 862},
{"COY", 96},
{"CRAB", 863},
{"CRAG", 864},
{"CRAM", 865},
{"CRAY", 866},
{"CREW", 867},
{"CRIB", 868},
{"CROW", 869},
{"CRUD", 870},
{"CRY", 97},
{"CUB", 98},
{"CUBA", 871},
{"CUBE", 872},
{"CUE", 99},
{"CUFF", 873},
{"CULL", 874},
{"CULT", 875},
{"CUNY", 876},
{"CUP", 100},
{"CUR", 101},
{"CURB", 877},
{"CURD", 878},
{"CURE", 879},
{"CURL", 880},
{"CURT", 881},
{"CUT", 102},
{"CUTS", 882},
{"DAB", 103},
{"DAD", 104},
{"DADE", 883},
{"DALE", 884},
{"DAM", 105},
{"DAME", 885},
{"DAN", 106},
{"DANA", 886},
{"DANE", 887},
{"DANG", 888},
{"DANK", 889},
{"DAR", 107},
{"DARE", 890},
{"DARK", 891},
{"DARN", 892},
{"DART", 893},
{"DASH", 894},
{"DATA", 895},
{"DATE", 896},
{"DAVE", 897},
{"DAVY", 898},
{"DAWN", 899},
{"DAY", 108},
{"DAYS", 900},
{"DEAD", 901},
{"DEAF", 902},
{"DEAL", 903},
{"DEAN", 904},
{"DEAR", 905},
{"DEBT", 906},
{"DECK", 907},
{"DEE", 109},
{"DEED", 908},
{"DEEM", 909},
{"DEER", 910},
{"DEFT", 911},
{"DEFY", 912},
{"DEL", 110},
{"DELL", 913},
{"DEN", 111},
{"DENT", 914},
{"DENY", 915},
{"DES", 112},
{"DESK", 916},
{"DEW", 113},
{"DIAL", 917},
{"DICE", 918},
{"DID", 114},
{"DIE", 115},
{"DIED", 919},
{"DIET", 920},
{"DIG", 116},
{"DIME", 921},
{"DIN", 117},
{"DINE", 922},
{"DING", 923},
{"DINT", 924},
{"DIP", 118},
{"DIRE", 925},
{"DIRT", 926},
{"DISC", 927},
{"DISH", 928},
{"DISK", 929},
{"DIVE", 930},
{"DO", 119},
{"DOCK", 931},
{"DOE", 120},
{"DOES", 932},
{"DOG", 121},
{"DOLE", 933},
{"DOLL", 934},
{"DOLT", 935},
{"DOME", 936},
{"DON", 122},
{"DONE", 937},
{"DOOM", 938},
{"DOOR", 939},
{"DORA", 940},
{"DOSE", 941},
{"DOT", 123},
{"DOTE", 942},
{"DOUG", 943},
{"DOUR", 944},
{"DOVE", 945},
{"DOW", 124},
{"DOWN", 946},
{"DRAB", 947},
{"DRAG", 948},
{"DRAM", 949},
{"DRAW", 950},
{"DREW", 951},
{"DRUB", 952},
{"DRUG", 953},
{"DRUM", 954},
{"DRY", 125},
{"DUAL", 955},
{"DUB", 126},
{"DUCK", 956},
{"DUCT", 957},
{"DUD", 127},
{"DUE", 128},
{"DUEL", 958},
{"DUET", 959},
{"DUG", 129},
{"DUKE", 960},
{"DULL", 961},
{"DUMB", 962},
{"DUN", 130},
{"DUNE", 963},
{"DUNK", 964},
{"DUSK", 965},
{"DUST", 966},
{"DUTY", 967},
{"EACH", 968},
{"EAR", 131},
{"EARL", 969},
{"EARN", 970},
{"EASE", 971},
{"EAST", 972},
{"EASY", 973},
{"EAT", 132},
{"EBEN", 974},
{"ECHO", 975},
{"ED", 133},
{"EDDY", 976},
{"EDEN", 977},
{"EDGE", 978},
{"EDGY", 979},
{"EDIT", 980},
{"EDNA", 981},
{"EEL", 134},
{"EGAN", 982},
{"EGG", 135},
{"EGO", 136},
{"ELAN", 983},
{"ELBA", 984},
{"ELI", 137},
{"ELK", 138},
{"ELLA", 985},
{"ELM", 139},
{"ELSE", 986},
{"ELY", 140},
{"EM", 141},
{"EMIL", 987},
{"EMIT", 988},
{"EMMA", 989},
{"END", 142},
{"ENDS", 990},
{"ERIC", 991},
{"EROS", 992},
{"EST", 143},
{"ETC", 144},
{"EVA", 145},
{"EVE", 146},
{"EVEN", 993},
{"EVER", 994},
{"EVIL", 995},
{"EWE", 147},
{"EYE", 148},
{"EYED", 996},
{"FACE", 997},
{"FACT", 998},
{"FAD", 149},
{"FADE", 999},
{"FAIL", 1000},
{"FAIN", 1001},
{"FAIR", 1002},
{"FAKE", 1003},
{"FALL", 1004},
{"FAME", 1005},
{"FAN", 150},
{"FANG", 1006},
{"FAR", 151},
{"FARM", 1007},
{"FAST", 1008},
{"FAT", 152},
{"FATE", 1009},
{"FAWN", 1010},
{"FAY", 153},
{"FEAR", 1011},
{"FEAT", 1012},
{"FED", 154},
{"FEE", 155},
{"FEED", 1013},
{"FEEL", 1014},
{"FEET", 1015},
{"FELL", 1016},
{"FELT", 1017},
{"FEND", 1018},
{"FERN", 1019},
{"FEST", 1020},
{"FEUD", 1021},
{"FEW", 156},
{"FIB", 157},
{"FIEF", 1022},
{"FIG", 158},
{"FIGS", 1023},
{"FILE", 1024},
{"FILL", 1025},
{"FILM", 1026},
{"FIN", 159},
{"FIND", 1027},
{"FINE", 1028},
{"FINK", 1029},
{"FIR", 160},
{"FIRE", 1030},
{"FIRM", 1031},
{"FISH", 1032},
{"FISK", 1033},
{"FIST", 1034},
{"FIT", 161},
{"FITS", 1035},
{"FIVE", 1036},
{"FLAG", 1037},
{"FLAK", 1038},
{"FLAM", 1039},
{"FLAT", 1040},
{"FLAW", 1041},
{"FLEA", 1042},
{"FLED", 1043},
{"FLEW", 1044},
{"FLIT", 1045},
{"FLO", 162},
{"FLOC", 1046},
{"FLOG", 1047},
{"FLOW", 1048},
{"FLUB", 1049},
{"FLUE", 1050},
{"FLY", 163},
{"FOAL", 1051},
{"FOAM", 1052},
{"FOE", 164},
{"FOG", 165},
{"FOGY", 1053},
{"FOIL", 1054},
{"FOLD", 1055},
{"FOLK", 1056},
{"FOND", 1057},
{"FONT", 1058},
{"FOOD", 1059},
{"FOOL", 1060},
{"FOOT", 1061},
{"FOR", 166},
{"FORD", 1062},
{"FORE", 1063},
{"FORK", 1064},
{"FORM", 1065},
{"FORT", 1066},
{"FOSS", 1067},
{"FOUL", 1068},
{"FOUR", 1069},
{"FOWL", 1070},
{"FRAU", 1071},
{"FRAY", 1072},
{"FRED", 1073},
{"FREE", 1074},
{"FRET", 1075},
{"FREY", 1076},
{"FROG", 1077},
{"FROM", 1078},
{"FRY", 167},
{"FUEL", 1079},
{"FULL", 1080},
{"FUM", 168},
{"FUME", 1081},
{"FUN", 169},
{"FUND", 1082},
{"FUNK", 1083},
{"FUR", 170},
{"FURY", 1084},
{"FUSE", 1085},
{"FUSS", 1086},
{"GAB", 171},
{"GAD", 172},
{"GAFF", 1087},
{"GAG", 173},
{"GAGE", 1088},
{"GAIL", 1089},
{"GAIN", 1090},
{"GAIT", 1091},
{"GAL", 174},
{"GALA", 1092},
{"GALE", 1093},
{"GALL", 1094},
{"GALT", 1095},
{"GAM", 175},
{"GAME", 1096},
{"GANG", 1097},
{"GAP", 176},
{"GARB", 1098},
{"GARY", 1099},
{"GAS", 177},
{"GASH", 1100},
{"GATE", 1101},
{"GAUL", 1102},
{"GAUR", 1103},
{"GAVE", 1104},
{"GAWK", 1105},
{"GAY", 178},
{"GEAR", 1106},
{"GEE", 179},
{"GEL", 180},
{"GELD", 1107},
{"GEM", 181},
{"GENE", 1108},
{"GENT", 1109},
{"GERM", 1110},
{"GET", 182},
{"GETS", 1111},
{"GIBE", 1112},
{"GIFT", 1113},
{"GIG", 183},
{"GIL", 184},
{"GILD", 1114},
{"GILL", 1115},
{"GILT", 1116},
{"GIN", 185},
{"GINA", 1117},
{"GIRD", 1118},
{"GIRL", 1119},
{"GIST", 1120},
{"GIVE", 1121},
{"GLAD", 1122},
{"GLEE", 1123},
{"GLEN", 1124},
{"GLIB", 1125},
{"GLOB", 1126},
{"GLOM", 1127},
{"GLOW", 1128},
{"GLUE", 1129},
{"GLUM", 1130},
{"GLUT", 1131},
{"GO", 186},
{"GOAD", 1132},
{"GOAL", 1133},
{"GOAT", 1134},
{"GOER", 1135},
{"GOES", 1136},
{"GOLD", 1137},
{"GOLF", 1138},
{"GONE", 1139},
{"GONG", 1140},
{"GOOD", 1141},
{"GOOF", 1142},
{"GORE", 1143},
{"GORY", 1144},
{"GOSH", 1145},
{"GOT", 187},
{"GOUT", 1146},
{"GOWN", 1147},
{"GRAB", 1148},
{"GRAD", 1149},
{"GRAY", 1150},
{"GREG", 1151},
{"GREW", 1152},
{"GREY", 1153},
{"GRID", 1154},
{"GRIM", 1155},
{"GRIN", 1156},
{"GRIT", 1157},
{"GROW", 1158},
{"GRUB", 1159},
{"GULF", 1160},
{"GULL", 1161},
{"GUM", 188},
{"GUN", 189},
{"GUNK", 1162},
{"GURU", 1163},
{"GUS", 190},
{"GUSH", 1164},
{"GUST", 1165},
{"GUT", 191},
{"GUY", 192},
{"GWEN", 1166},
{"GWYN", 1167},
{"GYM", 193},
{"GYP", 194},
{"HA", 195},
{"HAAG", 1168},
{"HAAS", 1169},
{"HACK", 1170},
{"HAD", 196},
{"HAIL", 1171},
{"HAIR", 1172},
{"HAL", 197},
{"HALE", 1173},
{"HALF", 1174},
{"HALL", 1175},
{"HALO", 1176},
{"HALT", 1177},
{"HAM", 198},
{"HAN", 199},
{"HAND", 1178},
{"HANG", 1179},
{"HANK", 1180},
{"HANS", 1181},
{"HAP", 200},
{"HARD", 1182},
{"HARK", 1183},
{"HARM", 1184},
{"HART", 1185},
{"HAS", 201},
{"HASH", 1186},
{"HAST", 1187},
{"HAT", 202},
{"HATE", 1188},
{"HATH", 1189},
{"HAUL", 1190},
{"HAVE", 1191},
{"HAW", 203},
{"HAWK", 1192},
{"HAY", 204},
{"HAYS", 1193},
{"HE", 205},
{"HEAD", 1194},
{"HEAL", 1195},
{"HEAR", 1196},
{"HEAT", 1197},
{"HEBE", 1198},
{"HECK", 1199},
{"HEED", 1200},
{"HEEL", 1201},
{"HEFT", 1202},
{"HELD", 1203},
{"HELL", 1204},
{"HELM", 1205},
{"HEM", 206},
{"HEN", 207},
{"HER", 208},
{"HERB", 1206},
{"HERD", 1207},
{"HERE", 1208},
{"HERO", 1209},
{"HERS", 1210},
{"HESS", 1211},
{"HEW", 209},
{"HEWN", 1212},
{"HEY", 210},
{"HI", 211},
{"HICK", 1213},
{"HID", 212},
{"HIDE", 1214},
{"HIGH", 1215},
{"HIKE", 1216},
{"HILL", 1217},
{"HILT", 1218},
{"HIM", 213},
{"HIND", 1219},
{"HINT", 1220},
{"HIP", 214},
{"HIRE", 1221},
{"HIS", 215},
{"HISS", 1222},
{"HIT", 216},
{"HIVE", 1223},
{"HO", 217},
{"HOB", 218},
{"HOBO", 1224},
{"HOC", 219},
{"HOCK", 1225},
{"HOE", 220},
{"HOFF", 1226},
{"HOG", 221},
{"HOLD", 1227},
{"HOLE", 1228},
{"HOLM", 1229},
{"HOLT", 1230},
{"HOME", 1231},
{"HONE", 1232},
{"HONK", 1233},
{"HOOD", 1234},
{"HOOF", 1235},
{"HOOK", 1236},
{"HOOT", 1237},
{"HOP", 222},
{"HORN", 1238},
{"HOSE", 1239},
{"HOST", 1240},
{"HOT", 223},
{"HOUR", 1241},
{"HOVE", 1242},
{"HOW", 224},
{"HOWE", 1243},
{"HOWL", 1244},
{"HOYT", 1245},
{"HUB", 225},
{"HUCK", 1246},
{"HUE", 226},
{"HUED", 1247},
{"HUFF", 1248},
{"HUG", 227},
{"HUGE", 1249},
{"HUGH", 1250},
{"HUGO", 1251},
{"HUH", 228},
{"HULK", 1252},
{"HULL", 1253},
{"HUM", 229},
{"HUNK", 1254},
{"HUNT", 1255},
{"HURD", 1256},
{"HURL", 1257},
{"HURT", 1258},
{"HUSH", 1259},
{"HUT", 230},
{"HYDE", 1260},
{"HYMN", 1261},
{"I", 231},
{"IBIS", 1262},
{"ICON", 1263},
{"ICY", 232},
{"IDA", 233},
{"IDEA", 1264},
{"IDLE", 1265},
{"IF", 234},
{"IFFY", 1266},
{"IKE", 235},
{"ILL", 236},
{"INCA", 1267},
{"INCH", 1268},
{"INK", 237},
{"INN", 238},
{"INTO", 1269},
{"IO", 239},
{"ION", 240},
{"IONS", 1270},
{"IOTA", 1271},
{"IOWA", 1272},
{"IQ", 241},
{"IRA", 242},
{"IRE", 243},
{"IRIS", 1273},
{"IRK", 244},
{"IRMA", 1274},
{"IRON", 1275},
{"IS", 245},
{"ISLE", 1276},
{"IT", 246},
{"ITCH", 1277},
{"ITEM", 1278},
{"ITS", 247},
{"IVAN", 1279},
{"IVY", 248},
{"JAB", 249},
{"JACK", 1280},
{"JADE", 1281},
{"JAG", 250},
{"JAIL", 1282},
{"JAKE", 1283},
{"JAM", 251},
{"JAN", 252},
{"JANE", 1284},
{"JAR", 253},
{"JAVA", 1285},
{"JAW", 254},
{"JAY", 255},
{"JEAN", 1286},
{"JEFF", 1287},
{"JERK", 1288},
{"JESS", 1289},
{"JEST", 1290},
{"JET", 256},
{"JIBE", 1291},
{"JIG", 257},
{"JILL", 1292},
{"JILT", 1293},
{"JIM", 258},
{"JIVE", 1294},
{"JO", 259},
{"JOAN", 1295},
{"JOB", 260},
{"JOBS", 1296},
{"JOCK", 1297},
{"JOE", 261},
{"JOEL", 1298},
{"JOEY", 1299},
{"JOG", 262},
{"JOHN", 1300},
{"JOIN", 1301},
{"JOKE", 1302},
{"JOLT", 1303},
{"JOT", 263},
{"JOVE", 1304},
{"JOY", 264},
{"JUDD", 1305},
{"JUDE", 1306},
{"JUDO", 1307},
{"JUDY", 1308},
{"JUG", 265},
{"JUJU", 1309},
{"JUKE", 1310},
{"JULY", 1311},
{"JUNE", 1312},
{"JUNK", 1313},
{"JUNO", 1314},
{"JURY", 1315},
{"JUST", 1316},
{"JUT", 266},
{"JUTE", 1317},
{"KAHN", 1318},
{"KALE", 1319},
{"KANE", 1320},
{"KANT", 1321},
{"KARL", 1322},
{"KATE", 1323},
{"KAY", 267},
{"KEEL", 1324},
{"KEEN", 1325},
{"KEG", 268},
{"KEN", 269},
{"KENO", 1326},
{"KENT", 1327},
{"KERN", 1328},
{"KERR", 1329},
{"KEY", 270},
{"KEYS", 1330},
{"KICK", 1331},
{"KID", 271},
{"KILL", 1332},
{"KIM", 272},
{"KIN", 273},
{"KIND", 1333},
{"KING", 1334},
{"KIRK", 1335},
{"KISS", 1336},
{"KIT", 274},
{"KITE", 1337},
{"KLAN", 1338},
{"KNEE", 1339},
{"KNEW", 1340},
{"KNIT", 1341},
{"KNOB", 1342},
{"KNOT", 1343},
{"KNOW", 1344},
{"KOCH", 1345},
{"KONG", 1346},
{"KUDO", 1347},
{"KURD", 1348},
{"KURT", 1349},
{"KYLE", 1350},
{"LA", 275},
{"LAB", 276},
{"LAC", 277},
{"LACE", 1351},
{"LACK", 1352},
{"LACY", 1353},
{"LAD", 278},
{"LADY", 1354},
{"LAG", 279},
{"LAID", 1355},
{"LAIN", 1356},
{"LAIR", 1357},
{"LAKE", 1358},
{"LAM", 280},
{"LAMB", 1359},
{"LAME", 1360},
{"LAND", 1361},
{"LANE", 1362},
{"LANG", 1363},
{"LAP", 281},
{"LARD", 1364},
{"LARK", 1365},
{"LASS", 1366},
{"LAST", 1367},
{"LATE", 1368},
{"LAUD", 1369},
{"LAVA", 1370},
{"LAW", 282},
{"LAWN", 1371},
{"LAWS", 1372},
{"LAY", 283},
{"LAYS", 1373},
{"LEA", 284},
{"LEAD", 1374},
{"LEAF", 1375},
{"LEAK", 1376},
{"LEAN", 1377},
{"LEAR", 1378},
{"LED", 285},
{"LEE", 286},
{"LEEK", 1379},
{"LEER", 1380},
{"LEFT", 1381},
{"LEG", 287},
{"LEN", 288},
{"LEND", 1382},
{"LENS", 1383},
{"LENT", 1384},
{"LEO", 289},
{"LEON", 1385},
{"LESK", 1386},
{"LESS", 1387},
{"LEST", 1388},
{"LET", 290},
{"LETS", 1389},
{"LEW", 291},
{"LIAR", 1390},
{"LICE", 1391},
{"LICK", 1392},
{"LID", 292},
{"LIE", 293},
{"LIED", 1393},
{"LIEN", 1394},
{"LIES", 1395},
{"LIEU", 1396},
{"LIFE", 1397},
{"LIFT", 1398},
{"LIKE", 1399},
{"LILA", 1400},
{"LILT", 1401},
{"LILY", 1402},
{"LIMA", 1403},
{"LIMB", 1404},
{"LIME", 1405},
{"LIN", 294},
{"LIND", 1406},
{"LINE", 1407},
{"LINK", 1408},
{"LINT", 1409},
{"LION", 1410},
{"LIP", 295},
{"LISA", 1411},
{"LIST", 1412},
{"LIT", 296},
{"LIVE", 1413},
{"LO", 297},
{"LOAD", 1414},
{"LOAF", 1415},
{"LOAM", 1416},
{"LOAN", 1417},
{"LOB", 298},
{"LOCK", 1418},
{"LOFT", 1419},
{"LOG", 299},
{"LOGE", 1420},
{"LOIS", 1421},
{"LOLA", 1422},
{"LONE", 1423},
{"LONG", 1424},
{"LOOK", 1425},
{"LOON", 1426},
{"LOOT", 1427},
{"LOP", 300},
{"LORD", 1428},
{"LORE", 1429},
{"LOS", 301},
{"LOSE", 1430},
{"LOSS", 1431},
{"LOST", 1432},
{"LOT", 302},
{"LOU", 303},
{"LOUD", 1433},
{"LOVE", 1434},
{"LOW", 304},
{"LOWE", 1435},
{"LOY", 305},
{"LUCK", 1436},
{"LUCY", 1437},
{"LUG", 306},
{"LUGE", 1438},
{"LUKE", 1439},
{"LULU", 1440},
{"LUND", 1441},
{"LUNG", 1442},
{"LURA", 1443},
{"LURE", 1444},
{"LURK", 1445},
{"LUSH", 1446},
{"LUST", 1447},
{"LYE", 307},
{"LYLE", 1448},
{"LYNN", 1449},
{"LYON", 1450},
{"LYRA", 1451},
{"MA", 308},
{"MAC", 309},
{"MACE", 1452},
{"MAD", 310},
{"MADE", 1453},
{"MAE", 311},
{"MAGI", 1454},
{"MAID", 1455},
{"MAIL", 1456},
{"MAIN", 1457},
{"MAKE", 1458},
{"MALE", 1459},
{"MALI", 1460},
{"MALL", 1461},
{"MALT", 1462},
{"MAN", 312},
{"MANA", 1463},
{"MANN", 1464},
{"MANY", 1465},
{"MAO", 313},
{"MAP", 314},
{"MARC", 1466},
{"MARE", 1467},
{"MARK", 1468},
{"MARS", 1469},
{"MART", 1470},
{"MARY", 1471},
{"MASH", 1472},
{"MASK", 1473},
{"MASS", 1474},
{"MAST", 1475},
{"MAT", 315},
{"MATE", 1476},
{"MATH", 1477},
{"MAUL", 1478},
{"MAW", 316},
{"MAY", 317},
{"MAYO", 1479},
{"ME", 318},
{"MEAD", 1480},
{"MEAL", 1481},
{"MEAN", 1482},
{"MEAT", 1483},
{"MEEK", 1484},
{"MEET", 1485},
{"MEG", 319},
{"MEL", 320},
{"MELD", 1486},
{"MELT", 1487},
{"MEMO", 1488},
{"MEN", 321},
{"MEND", 1489},
{"MENU", 1490},
{"MERT", 1491},
{"MESH", 1492},
{"MESS", 1493},
{"MET", 322},
{"MEW", 323},
{"MICE", 1494},
{"MID", 324},
{"MIKE", 1495},
{"MILD", 1496},
{"MILE", 1497},
{"MILK", 1498},
{"MILL", 1499},
{"MILT", 1500},
{"MIMI", 1501},
{"MIN", 325},
{"MIND", 1502},
{"MINE", 1503},
{"MINI", 1504},
{"MINK", 1505},
{"MINT", 1506},
{"MIRE", 1507},
{"MISS", 1508},
{"MIST", 1509},
{"MIT", 326},
{"MITE", 1510},
{"MITT", 1511},
{"MOAN", 1512},
{"MOAT", 1513},
{"MOB", 327},
{"MOCK", 1514},
{"MOD", 328},
{"MODE", 1515},
{"MOE", 329},
{"MOLD", 1516},
{"MOLE", 1517},
{"MOLL", 1518},
{"MOLT", 1519},
{"MONA", 1520},
{"MONK", 1521},
{"MONT", 1522},
{"MOO", 330},
{"MOOD", 1523},
{"MOON", 1524},
{"MOOR", 1525},
{"MOOT", 1526},
{"MOP", 331},
{"MORE", 1527},
{"MORN", 1528},
{"MORT", 1529},
{"MOS", 332},
{"MOSS", 1530},
{"MOST", 1531},
{"MOT", 333},
{"MOTH", 1532},
{"MOVE", 1533},
{"MOW", 334},
{"MUCH", 1534},
{"MUCK", 1535},
{"MUD", 335},
{"MUDD", 1536},
{"MUFF", 1537},
{"MUG", 336},
{"MULE", 1538},
{"MULL", 1539},
{"MUM", 337},
{"MURK", 1540},
{"MUSH", 1541},
{"MUST", 1542},
{"MUTE", 1543},
{"MUTT", 1544},
{"MY", 338},
{"MYRA", 1545},
{"MYTH", 1546},
{"NAB", 339},
{"NAG", 340},
{"NAGY", 1547},
{"NAIL", 1548},
{"NAIR", 1549},
{"NAME", 1550},
{"NAN", 341},
{"NAP", 342},
{"NARY", 1551},
{"NASH", 1552},
{"NAT", 343},
{"NAVE", 1553},
{"NAVY", 1554},
{"NAY", 344},
{"NE", 345},
{"NEAL", 1555},
{"NEAR", 1556},
{"NEAT", 1557},
{"NECK", 1558},
{"NED", 346},
{"NEE", 347},
{"NEED", 1559},
{"NEIL", 1560},
{"NELL", 1561},
{"NEON", 1562},
{"NERO", 1563},
{"NESS", 1564},
{"NEST", 1565},
{"NET", 348},
{"NEW", 349},
{"NEWS", 1566},
{"NEWT", 1567},
{"NIB", 350},
{"NIBS", 1568},
{"NICE", 1569},
{"NICK", 1570},
{"NIIL", 351},
{"NILE", 1571},
{"NINA", 1572},
{"NINE", 1573},
{"NIP", 352},
{"NIT", 353},
{"NO", 354},
{"NOAH", 1574},
{"NOB", 355},
{"NOD", 356},
{"NODE", 1575},
{"NOEL", 1576},
{"NOLL", 1577},
{"NON", 357},
{"NONE", 1578},
{"NOOK", 1579},
{"NOON", 1580},
{"NOR", 358},
{"NORM", 1581},
{"NOSE", 1582},
{"NOT", 359},
{"NOTE", 1583},
{"NOUN", 1584},
{"NOV", 360},
{"NOVA", 1585},
{"NOW", 361},
{"NU", 362},
{"NUDE", 1586},
{"NULL", 1587},
{"NUMB", 1588},
{"NUN", 363},
{"NUT", 364},
{"O", 365},
{"OAF", 366},
{"OAK", 367},
{"OAR", 368},
{"OAT", 369},
{"OATH", 1589},
{"OBEY", 1590},
{"OBOE", 1591},
{"ODD", 370},
{"ODE", 371},
{"ODIN", 1592},
{"OF", 372},
{"OFF", 373},
{"OFT", 374},
{"OH", 375},
{"OHIO", 1593},
{"OIL", 376},
{"OILY", 1594},
{"OINT", 1595},
{"OK", 377},
{"OKAY", 1596},
{"OLAF", 1597},
{"OLD", 378},
{"OLDY", 1598},
{"OLGA", 1599},
{"OLIN", 1600},
{"OMAN", 1601},
{"OMEN", 1602},
{"OMIT", 1603},
{"ON", 379},
{"ONCE", 1604},
{"ONE", 380},
{"ONES", 1605},
{"ONLY", 1606},
{"ONTO", 1607},
{"ONUS", 1608},
{"OR", 381},
{"ORAL", 1609},
{"ORB", 382},
{"ORE", 383},
{"ORGY", 1610},
{"ORR", 384},
{"OS", 385},
{"OSLO", 1611},
{"OTIS", 1612},
{"OTT", 386},
{"OTTO", 1613},
{"OUCH", 1614},
{"OUR", 387},
{"OUST", 1615},
{"OUT", 388},
{"OUTS", 1616},
{"OVA", 389},
{"OVAL", 1617},
{"OVEN", 1618},
{"OVER", 1619},
{"OW", 390},
{"OWE", 391},
{"OWL", 392},
{"OWLY", 1620},
{"OWN", 393},
{"OWNS", 1621},
{"OX", 394},
{"PA", 395},
{"PAD", 396},
{"PAL", 397},
{"PAM", 398},
{"PAN", 399},
{"PAP", 400},
{"PAR", 401},
{"PAT", 402},
{"PAW", 403},
{"PAY", 404},
{"PEA", 405},
{"PEG", 406},
{"PEN", 407},
{"PEP", 408},
{"PER", 409},
{"PET", 410},
{"PEW", 411},
{"PHI", 412},
{"PI", 413},
{"PIE", 414},
{"PIN", 415},
{"PIT", 416},
{"PLY", 417},
{"PO", 418},
{"POD", 419},
{"POE", 420},
{"POP", 421},
{"POT", 422},
{"POW", 423},
{"PRO", 424},
{"PRY", 425},
{"PUB", 426},
{"PUG", 427},
{"PUN", 428},
{"PUP", 429},
{"PUT", 430},
{"QUAD", 1622},
{"QUIT", 1623},
{"QUO", 431},
{"QUOD", 1624},
{"RACE", 1625},
{"RACK", 1626},
{"RACY", 1627},
{"RAFT", 1628},
{"RAG", 432},
{"RAGE", 1629},
{"RAID", 1630},
{"RAIL", 1631},
{"RAIN", 1632},
{"RAKE", 1633},
{"RAM", 433},
{"RAN", 434},
{"RANK", 1634},
{"RANT", 1635},
{"RAP", 435},
{"RARE", 1636},
{"RASH", 1637},
{"RAT", 436},
{"RATE", 1638},
{"RAVE", 1639},
{"RAW", 437},
{"RAY", 438},
{"RAYS", 1640},
{"READ", 1641},
{"REAL", 1642},
{"REAM", 1643},
{"REAR", 1644},
{"REB", 439},
{"RECK", 1645},
{"RED", 440},
{"REED", 1646},
{"REEF", 1647},
{"REEK", 1648},
{"REEL", 1649},
{"REID", 1650},
{"REIN", 1651},
{"RENA", 1652},
{"REND", 1653},
{"RENT", 1654},
{"REP", 441},
{"REST", 1655},
{"RET", 442},
{"RIB", 443},
{"RICE", 1656},
{"RICH", 1657},
{"RICK", 1658},
{"RID", 444},
{"RIDE", 1659},
{"RIFT", 1660},
{"RIG", 445},
{"RILL", 1661},
{"RIM", 446},
{"RIME", 1662},
{"RING", 1663},
{"RINK", 1664},
{"RIO", 447},
{"RIP", 448},
{"RISE", 1665},
{"RISK", 1666},
{"RITE", 1667},
{"ROAD", 1668},
{"ROAM", 1669},
{"ROAR", 1670},
{"ROB", 449},
{"ROBE", 1671},
{"ROCK", 1672},
{"ROD", 450},
{"RODE", 1673},
{"ROE", 451},
{"ROIL", 1674},
{"ROLL", 1675},
{"ROME", 1676},
{"RON", 452},
{"ROOD", 1677},
{"ROOF", 1678},
{"ROOK", 1679},
{"ROOM", 1680},
{"ROOT", 1681},
{"ROSA", 1682},
{"ROSE", 1683},
{"ROSS", 1684},
{"ROSY", 1685},
{"ROT", 453},
{"ROTH", 1686},
{"ROUT", 1687},
{"ROVE", 1688},
{"ROW", 454},
{"ROWE", 1689},
{"ROWS", 1690},
{"ROY", 455},
{"RUB", 456},
{"RUBE", 1691},
{"RUBY", 1692},
{"RUDE", 1693},
{"RUDY", 1694},
{"RUE", 457},
{"RUG", 458},
{"RUIN", 1695},
{"RULE", 1696},
{"RUM", 459},
{"RUN", 460},
{"RUNG", 1697},
{"RUNS", 1698},
{"RUNT", 1699},
{"RUSE", 1700},
{"RUSH", 1701},
{"RUSK", 1702},
{"RUSS", 1703},
{"RUST", 1704},
{"RUTH", 1705},
{"RYE", 461},
{"SAC", 462},
{"SACK", 1706},
{"SAD", 463},
{"SAFE", 1707},
{"SAG", 464},
{"SAGE", 1708},
{"SAID", 1709},
{"SAIL", 1710},
{"SAL", 465},
{"SALE", 1711},
{"SALK", 1712},
{"SALT", 1713},
{"SAM", 466},
{"SAME", 1714},
{"SAN", 467},
{"SAND", 1715},
{"SANE", 1716},
{"SANG", 1717},
{"SANK", 1718},
{"SAP", 468},
{"SARA", 1719},
{"SAT", 469},
{"SAUL", 1720},
{"SAVE", 1721},
{"SAW", 470},
{"SAY", 471},
{"SAYS", 1722},
{"SCAN", 1723},
{"SCAR", 1724},
{"SCAT", 1725},
{"SCOT", 1726},
{"SEA", 472},
{"SEAL", 1727},
{"SEAM", 1728},
{"SEAR", 1729},
{"SEAT", 1730},
{"SEC", 473},
{"SEE", 474},
{"SEED", 1731},
{"SEEK", 1732},
{"SEEM", 1733},
{"SEEN", 1734},
{"SEES", 1735},
{"SELF", 1736},
{"SELL", 1737},
{"SEN", 475},
{"SEND", 1738},
{"SENT", 1739},
{"SET", 476},
{"SETS", 1740},
{"SEW", 477},
{"SEWN", 1741},
{"SHAG", 1742},
{"SHAM", 1743},
{"SHAW", 1744},
{"SHAY", 1745},
{"SHE", 478},
{"SHED", 1746},
{"SHIM", 1747},
{"SHIN", 1748},
{"SHOD", 1749},
{"SHOE", 1750},
{"SHOT", 1751},
{"SHOW", 1752},
{"SHUN", 1753},
{"SHUT", 1754},
{"SHY", 479},
{"SICK", 1755},
{"SIDE", 1756},
{"SIFT", 1757},
{"SIGH", 1758},
{"SIGN", 1759},
{"SILK", 1760},
{"SILL", 1761},
{"SILO", 1762},
{"SILT", 1763},
{"SIN", 480},
{"SINE", 1764},
{"SING", 1765},
{"SINK", 1766},
{"SIP", 481},
{"SIR", 482},
{"SIRE", 1767},
{"SIS", 483},
{"SIT", 484},
{"SITE", 1768},
{"SITS", 1769},
{"SITU", 1770},
{"SKAT", 1771},
{"SKEW", 1772},
{"SKI", 485},
{"SKID", 1773},
{"SKIM", 1774},
{"SKIN", 1775},
{"SKIT", 1776},
{"SKY", 486},
{"SLAB", 1777},
{"SLAM", 1778},
{"SLAT", 1779},
{"SLAY", 1780},
{"SLED", 1781},
{"SLEW", 1782},
{"SLID", 1783},
{"SLIM", 1784},
{"SLIT", 1785},
{"SLOB", 1786},
{"SLOG", 1787},
{"SLOT", 1788},
{"SLOW", 1789},
{"SLUG", 1790},
{"SLUM", 1791},
{"SLUR", 1792},
{"SLY", 487},
{"SMOG", 1793},
{"SMUG", 1794},
{"SNAG", 1795},
{"SNOB", 1796},
{"SNOW", 1797},
{"SNUB", 1798},
{"SNUG", 1799},
{"SO", 488},
{"SOAK", 1800},
{"SOAR", 1801},
{"SOB", 489},
{"SOCK", 1802},
{"SOD", 490},
{"SODA", 1803},
{"SOFA", 1804},
{"SOFT", 1805},
{"SOIL", 1806},
{"SOLD", 1807},
{"SOME", 1808},
{"SON", 491},
{"SONG", 1809},
{"SOON", 1810},
{"SOOT", 1811},
{"SOP", 492},
{"SORE", 1812},
{"SORT", 1813},
{"SOUL", 1814},
{"SOUR", 1815},
{"SOW", 493},
{"SOWN", 1816},
{"SOY", 494},
{"SPA", 495},
{"SPY", 496},
{"STAB", 1817},
{"STAG", 1818},
{"STAN", 1819},
{"STAR", 1820},
{"STAY", 1821},
{"STEM", 1822},
{"STEW", 1823},
{"STIR", 1824},
{"STOW", 1825},
{"STUB", 1826},
{"STUN", 1827},
{"SUB", 497},
{"SUCH", 1828},
{"SUD", 498},
{"SUDS", 1829},
{"SUE", 499},
{"SUIT", 1830},
{"SULK", 1831},
{"SUM", 500},
{"SUMS", 1832},
{"SUN", 501},
{"SUNG", 1833},
{"SUNK", 1834},
{"SUP", 502},
{"SURE", 1835},
{"SURF", 1836},
{"SWAB", 1837},
{"SWAG", 1838},
{"SWAM", 1839},
{"SWAN", 1840},
{"SWAT", 1841},
{"SWAY", 1842},
{"SWIM", 1843},
{"SWUM", 1844},
{"TAB", 503},
{"TACK", 1845},
{"TACT", 1846},
{"TAD", 504},
{"TAG", 505},
{"TAIL", 1847},
{"TAKE", 1848},
{"TALE", 1849},
{"TALK", 1850},
{"TALL", 1851},
{"TAN", 506},
{"TANK", 1852},
{"TAP", 507},
{"TAR", 508},
{"TASK", 1853},
{"TATE", 1854},
{"TAUT", 1855},
{"TEA", 509},
{"TEAL", 1856},
{"TEAM", 1857},
{"TEAR", 1858},
{"TECH", 1859},
{"TED", 510},
{"TEE", 511},
{"TEEM", 1860},
{"TEEN", 1861},
{"TEET", 1862},
{"TELL", 1863},
{"TEN", 512},
{"TEND", 1864},
{"TENT", 1865},
{"TERM", 1866},
{"TERN", 1867},
{"TESS", 1868},
{"TEST", 1869},
{"THAN", 1870},
{"THAT", 1871},
{"THE", 513},
{"THEE", 1872},
{"THEM", 1873},
{"THEN", 1874},
{"THEY", 1875},
{"THIN", 1876},
{"THIS", 1877},
{"THUD", 1878},
{"THUG", 1879},
{"THY", 514},
{"TIC", 515},
{"TICK", 1880},
{"TIDE", 1881},
{"TIDY", 1882},
{"TIE", 516},
{"TIED", 1883},
{"TIER", 1884},
{"TILE", 1885},
{"TILL", 1886},
{"TILT", 1887},
{"TIM", 517},
{"TIME", 1888},
{"TIN", 518},
{"TINA", 1889},
{"TINE", 1890},
{"TINT", 1891},
{"TINY", 1892},
{"TIP", 519},
{"TIRE", 1893},
{"TO", 520},
{"TOAD", 1894},
{"TOE", 521},
{"TOG", 522},
{"TOGO", 1895},
{"TOIL", 1896},
{"TOLD", 1897},
{"TOLL", 1898},
{"TOM", 523},
{"TON", 524},
{"TONE", 1899},
{"TONG", 1900},
{"TONY", 1901},
{"TOO", 525},
{"TOOK", 1902},
{"TOOL", 1903},
{"TOOT", 1904},
{"TOP", 526},
{"TORE", 1905},
{"TORN", 1906},
{"TOTE", 1907},
{"TOUR", 1908},
{"TOUT", 1909},
{"TOW", 527},
{"TOWN", 1910},
{"TOY", 528},
{"TRAG", 1911},
{"TRAM", 1912},
{"TRAY", 1913},
{"TREE", 1914},
{"TREK", 1915},
{"TRIG", 1916},
{"TRIM", 1917},
{"TRIO", 1918},
{"TROD", 1919},
{"TROT", 1920},
{"TROY", 1921},
{"TRUE", 1922},
{"TRY", 529},
{"TUB", 530},
{"TUBA", 1923},
{"TUBE", 1924},
{"TUCK", 1925},
{"TUFT", 1926},
{"TUG", 531},
{"TUM", 532},
{"TUN", 533},
{"TUNA", 1927},
{"TUNE", 1928},
{"TUNG", 1929},
{"TURF", 1930},
{"TURN", 1931},
{"TUSK", 1932},
{"TWIG", 1933},
{"TWIN", 1934},
{"TWIT", 1935},
{"TWO", 534},
{"ULAN", 1936},
{"UN", 535},
{"UNIT", 1937},
{"UP", 536},
{"URGE", 1938},
{"US", 537},
{"USE", 538},
{"USED", 1939},
{"USER", 1940},
{"USES", 1941},
{"UTAH", 1942},
{"VAIL", 1943},
{"VAIN", 1944},
{"VALE", 1945},
{"VAN", 539},
{"VARY", 1946},
{"VASE", 1947},
{"VAST", 1948},
{"VAT", 540},
{"VEAL", 1949},
{"VEDA", 1950},
{"VEIL", 1951},
{"VEIN", 1952},
{"VEND", 1953},
{"VENT", 1954},
{"VERB", 1955},
{"VERY", 1956},
{"VET", 541},
{"VETO", 1957},
{"VICE", 1958},
{"VIE", 542},
{"VIEW", 1959},
{"VINE", 1960},
{"VISE", 1961},
{"VOID", 1962},
{"VOLT", 1963},
{"VOTE", 1964},
{"WACK", 1965},
{"WAD", 543},
{"WADE", 1966},
{"WAG", 544},
{"WAGE", 1967},
{"WAIL", 1968},
{"WAIT", 1969},
{"WAKE", 1970},
{"WALE", 1971},
{"WALK", 1972},
{"WALL", 1973},
{"WALT", 1974},
{"WAND", 1975},
{"WANE", 1976},
{"WANG", 1977},
{"WANT", 1978},
{"WAR", 545},
{"WARD", 1979},
{"WARM", 1980},
{"WARN", 1981},
{"WART", 1982},
{"WAS", 546},
{"WASH", 1983},
{"WAST", 1984},
{"WATS", 1985},
{"WATT", 1986},
{"WAVE", 1987},
{"WAVY", 1988},
{"WAY", 547},
{"WAYS", 1989},
{"WE", 548},
{"WEAK", 1990},
{"WEAL", 1991},
{"WEAN", 1992},
{"WEAR", 1993},
{"WEB", 549},
{"WED", 550},
{"WEE", 551},
{"WEED", 1994},
{"WEEK", 1995},
{"WEIR", 1996},
{"WELD", 1997},
{"WELL", 1998},
{"WELT", 1999},
{"WENT", 2000},
{"WERE", 2001},
{"WERT", 2002},
{"WEST", 2003},
{"WET", 552},
{"WHAM", 2004},
{"WHAT", 2005},
{"WHEE", 2006},
{"WHEN", 2007},
{"WHET", 2008},
{"WHO", 553},
{"WHOA", 2009},
{"WHOM", 2010},
{"WHY", 554},
{"WICK", 2011},
{"WIFE", 2012},
{"WILD", 2013},
{"WILL", 2014},
{"WIN", 555},
{"WIND", 2015},
{"WINE", 2016},
{"WING", 2017},
{"WINK", 2018},
{"WINO", 2019},
{"WIRE", 2020},
{"WISE", 2021},
{"WISH", 2022},
{"WIT", 556},
{"WITH", 2023},
{"WOK", 557},
{"WOLF", 2024},
{"WON", 558},
{"WONT", 2025},
{"WOO", 559},
{"WOOD", 2026},
{"WOOL", 2027},
{"WORD", 2028},
{"WORE", 2029},
{"WORK", 2030},
{"WORM", 2031},
{"WORN", 2032},
{"WOVE", 2033},
{"WOW", 560},
{"WRIT", 2034},
{"WRY", 561},
{"WU", 562},
{"WYNN", 2035},
{"YALE", 2036},
{"YAM", 563},
{"YANG", 2037},
{"YANK", 2038},
{"YAP", 564},
{"YARD", 2039},
{"YARN", 2040},
{"YAW", 565},
{"YAWL", 2041},
{"YAWN", 2042},
{"YE", 566},
{"YEA", 567},
{"YEAH", 2043},
{"YEAR", 2044},
{"YELL", 2045},
{"YES", 568},
{"YET", 569},
{"YOGA", 2046},
{"YOKE", 2047},
{"YOU", 570}
};

static int
cmp(const void *a, const void *b)
{
  struct e *e1, *e2;
  
  e1 = (struct e *)a;
  e2 = (struct e *)b;
  return strcasecmp (e1->s, e2->s);
}

static int
get_stdword (char *s, void *v)
{
  struct e e, *r;

  e.s = s;
  e.n = -1;
  r = (struct e *) bsearch (&e, inv_std_dict,
			    sizeof(inv_std_dict)/sizeof(*inv_std_dict),
			    sizeof(*inv_std_dict), cmp);
  if (r)
    return r->n;
  else
    return -1;
}

static void
compress (OtpKey key, unsigned wn[])
{
  key[0] = wn[0] >> 3;
  key[1] = ((wn[0] & 0x07) << 5) | (wn[1] >> 6);
  key[2] = ((wn[1] & 0x3F) << 2) | (wn[2] >> 9);
  key[3] = ((wn[2] >> 1) & 0xFF);
  key[4] = ((wn[2] & 0x01) << 7) | (wn[3] >> 4);
  key[5] = ((wn[3] & 0x0F) << 4) | (wn[4] >> 7);
  key[6] = ((wn[4] & 0x7F) << 1) | (wn[5] >> 10);
  key[7] = ((wn[5] >> 2) & 0xFF);
}

static int
get_altword (char *s, void *a)
{
  OtpAlgorithm *alg = (OtpAlgorithm *)a;
  int ret;
  unsigned char *res = malloc(alg->hashsize);

  if (res == NULL)
    return -1;
  alg->hash (s, strlen(s), res);
  ret = (unsigned)(res[alg->hashsize - 1]) | 
      ((res[alg->hashsize - 2] & 0x03) << 8);
  free (res);
  return ret;
}

static int
parse_words(unsigned wn[],
	    char *str,
	    int (*convert)(char *, void *),
	    void *arg)
{
  char *w, *wend, c;
  int i;

  w = str;
  for (i = 0; i < 6; ++i) {
    while (isspace(*w))
      ++w;
    wend = w;
    while (isalpha (*wend))
      ++wend;
    c = *wend;
    *wend = '\0';
    wn[i] = (*convert)(w, arg);
    *wend = c;
    w = wend;
    if (wn[i] < 0)
      return -1;
  }
  return 0;
}

int
otp_parse_stddict (OtpKey key, char *str)
{
  unsigned wn[6];

  if (parse_words (wn, str, get_stdword, NULL))
    return -1;
  compress (key, wn);
  if (otp_checksum (key) != (wn[5] & 0x03))
    return -1;
  return 0;
}

int
otp_parse_altdict (OtpKey key, char *str, OtpAlgorithm *alg)
{
  unsigned wn[6];

  if (parse_words (wn, str, get_altword, alg))
    return -1;
  compress (key, wn);
  if (otp_checksum (key) != (wn[5] & 0x03))
    return -1;
  return 0;
}

int
otp_parse_hex (OtpKey key, char *s)
{
  char buf[17], *b;
  int is[8];
  int i;

  b = buf;
  while (*s) {
    if (strchr ("0123456789ABCDEFabcdef", *s))
      if (b - buf >= 16)
	return -1;
      else
	*b++ = tolower(*s);
    s++;
  }
  *b = '\0';
  if (sscanf (buf, "%2x%2x%2x%2x%2x%2x%2x%2x",
	      &is[0], &is[1], &is[2], &is[3], &is[4],
	      &is[5], &is[6], &is[7]) != 8)
    return -1;
  for (i = 0; i < OTPKEYSIZE; ++i)
    key[i] = is[i];
  return 0;
}

int
otp_parse (OtpKey key, char *s, OtpAlgorithm *alg)
{
  int ret;
  int dohex = 1;

  if (strncmp (s, OTP_HEXPREFIX, strlen(OTP_HEXPREFIX)) == 0)
    return otp_parse_hex (key, s + strlen(OTP_HEXPREFIX));
  if (strncmp (s, OTP_WORDPREFIX, strlen(OTP_WORDPREFIX)) == 0) {
    s += strlen(OTP_WORDPREFIX);
    dohex = 0;
  }

  ret = otp_parse_stddict (key, s);
  if (ret)
    ret = otp_parse_altdict (key, s, alg);
  if (ret && dohex)
    ret = otp_parse_hex (key, s);
  return ret;
}
