#include "dictionary.h"

/* Look-Up-Tables */

const char *sourceLUT[] = {
    "STANDBY",
    "FM",
    "LW",
    "MW",
    "SW",
    "WEB",
    "BLUETOOTH"};

const char *isMinStepLUT[] = {
    "Normal",
    "Fine",
};

const char *isStereoLUT[] = {
    "Mono",
    "Stereo",
};

const char *noYesLUT[] = {
    "No",
    "Yes",
};

const char *offOnLUT[] = {
    "Off",
    "On",
};

const char *rdsModeLUT[] = {
    "Only realible",
    "Smart",
    "DX"};

/* ECC LUTs */
const static Country eccCountries1[] = {
    {"United States", "US", 0xA0},
    {"Anguilla", "AI", 0xA2},
    {"Bolivia", "BO", 0xA3},
    {"Guatemala", "GT", 0xA4},
    {"Cameroon", "CM", 0xD0},
    {"Namibia", "NA", 0xD1},
    {"Sierra Leone", "SL", 0xD2},
    {"Germany", "DE", 0xE0},
    {"Greece", "GR", 0xE1},
    {"Morocco", "MA", 0xE2},
    {"Moldova", "MD", 0xE4},
    {"Australia", "AU", 0xF0},
    {"Kiribati", "KI", 0xF1},
    {"Kuwait", "KW", 0xF2},
    {"Laos", "LA", 0xF3},
};

const static Country eccCountries2[] = {
    {"United States", "US", 0xA0},
    {"Antigua & B.", "AG", 0xA2},
    {"Colombia", "CO", 0xA3},
    {"Honduras", "HN", 0xA4},
    {"C. African R.", "CF", 0xD0},
    {"Liberia", "LR", 0xD1},
    {"Zimbabwe", "ZW", 0xD2},
    {"Algeria", "ALG", 0xE0},
    {"Algeria", "DZ", 0xE0},
    {"Cyprus", "CYP", 0xE1},
    {"Czechia", "CZE", 0xE2},
    {"Ireland", "IE", 0xE3},
    {"Estonia", "EE", 0xE4},
    {"Australia", "AU", 0xF0},
    {"Bhutan", "BT", 0xF1},
    {"Qatar", "QA", 0xF2},
    {"Thailand", "TH", 0xF3},
};

const static Country eccCountries3[] = {
    {"United States", "US", 0xA0},
    {"Ecuador", "EC", 0xA2},
    {"Jamaica", "JM", 0xA3},
    {"Aruba", "AW", 0xA4},
    {"Djibouti", "DJ", 0xD0},
    {"Ghana", "GH", 0xD1},
    {"Mozambique", "MZ", 0xD2},
    {"West Sahara", "EH", 0xD3},
    {"Andorra", "AND", 0xE0},
    {"San Marino", "SM", 0xE1},
    {"Poland", "PL", 0xE2},
    {"Turkey", "TR", 0xE3},
    {"Kyrghyzstan", "KG", 0xE4},
    {"Australia", "AU", 0xF0},
    {"Bangladesh", "BD", 0xF1},
    {"Cambodia", "KH", 0xF2},
    {"Tonga", "TO", 0xF3},
};

const static Country eccCountries4[] = {
    {"United States", "US", 0xA0},
    {"Falkland", "FK", 0xA2},
    {"Martinique", "MQ", 0xA3},
    {"Madagascar", "MG", 0xD0},
    {"Mauritania", "MR", 0xD1},
    {"Uganda", "UG", 0xD2},
    {"Israel", "IL", 0xE0},
    {"Switzerland", "CH", 0xE1},
    {"Vatican", "VA", 0xE2},
    {"Macedonia", "MK", 0xE3},
    {"Australia", "AU", 0xF0},
    {"Pakistan", "PK", 0xF1},
    {"West Samoa", "WS", 0xF2},
};

const static Country eccCountries5[] = {
    {"United States", "US", 0xA0},
    {"Barbados", "BB", 0xA2},
    {"Guiana", "GF", 0xA3},
    {"Montserrat", "MS", 0xA4},
    {"Mali", "ML", 0xD0},
    {"Sao Tome", "ST", 0xD1},
    {"Swaziland", "SZ", 0xD2},
    {"Rwanda", "RW", 0xD3},
    {"Italy", "IT", 0xE0},
    {"Jordan", "JO", 0xE1},
    {"Slovakia", "SK", 0xE2},
    {"Tajikistan", "TJ", 0xE3},
    {"Australia", "AU", 0xF0},
    {"Fiji", "FJ", 0xF1},
    {"India", "IN", 0xF2},
};

const static Country eccCountries6[] = {
    {"United States", "US", 0xA0},
    {"Belize", "BZ", 0xA2},
    {"Paraguay", "PY", 0xA3},
    {"Trinidad & T.", "TT", 0xA4},
    {"Angola", "AO", 0xD0},
    {"Cape Verde", "CV", 0xD1},
    {"Kenya", "KE", 0xD2},
    {"Lesotho", "LS", 0xD3},
    {"Belgium", "BEL", 0xE0},
    {"Finland", "FI", 0xE1},
    {"Syria", "SY", 0xE2},
    {"Yugoslavia", "YU", 0xE3},
    {"Ukraine", "UA", 0xE4},
    {"Ukraine", "UA", 0xE4},
    {"Australia", "AU", 0xF0},
    {"Oman", "OM", 0xF1},
    {"Macau", "MO", 0xF2},
};

const static Country eccCountries7[] = {
    {"United States", "US", 0xA0},
    {"Cayman Isl.", "KY", 0xA2},
    {"Nicaragua", "NI", 0xA3},
    {"Peru", "PE", 0xA4},
    {"E.  Guinea", "GQ", 0xD0},
    {"Senegal", "SN", 0xD1},
    {"Somalia", "SO", 0xD2},
    {"Russia", "RU", 0xE0},
    {"Luxembourg", "LU", 0xE1},
    {"Tunisia", "TN", 0xE2},
    {"Australia", "AU", 0xF0},
    {"Nauru", "NR", 0xF1},
    {"Vietnam", "VN", 0xF2},
};

const static Country eccCountries8[] = {
    {"United States", "US", 0xA0},
    {"Costa Rica", "CR", 0xA2},
    {"Puerto Rico", "PR", 0xA3},
    {"Suriname", "SR", 0xA4},
    {"Gabon", "GA", 0xD0},
    {"Gambia", "GM", 0xD1},
    {"Niger", "NE", 0xD2},
    {"Sechelles", "SC", 0xD3},
    {"Palestine", "PS", 0xE0},
    {"Bulgaria", "BUL", 0xE1},
    {"Madeira", "PT", 0xE2},
    {"Netherlands", "NL", 0xE3},
    {"Portugal", "PT", 0xE4},
    {"Iran", "IR", 0xF0},
    {"Philippines", "PH", 0xF2},
};

const static Country eccCountries9[] = {
    {"United States", "US", 0xA0},
    {"Cuba", "CU", 0xA2},
    {"Panama", "PA", 0xA3},
    {"Uruguay", "UY", 0xA4},
    {"Guinea", "GN", 0xD0},
    {"Burundi", "BI", 0xD1},
    {"Chad", "TD", 0xD2},
    {"Albania", "ALB", 0xE0},
    {"Denmark", "DNK", 0xE1},
    {"Liechtenstein", "LI", 0xE2},
    {"Latvia", "LV", 0xE3},
    {"Slovenia", "SI", 0xE4},
    {"Saudi Arabia", "SA", 0xF0},
    {"New Zealand", "NZ", 0xF1},
    {"Japan", "JP", 0xF2},
    {"P. New Guinea", "PG", 0xF3},
};

const static Country eccCountriesA[] = {
    {"United States", "US", 0xA0},
    {"Argentina", "AR", 0xA2},
    {"Dominica", "DM", 0xA3},
    {"Saint Kitts", "KN", 0xA4},
    {"South Africa", "ZA", 0xD0},
    {"Guinea-Bissau", "GW", 0xD2},
    {"Maurituis", "MU", 0xD3},
    {"Austria", "AUT", 0xE0},
    {"Gibraltar", "GI", 0xE1},
    {"Iceland", "IS", 0xE2},
    {"Lebanon", "LB", 0xE3},
    {"Armenia", "AM", 0xE4},
    {"Afghanistan", "AF", 0xF0},
    {"Soloman", "SB", 0xF1},
    {"Singapore", "SG", 0xF2},
};

const static Country eccCountriesB[] = {
    {"United States", "US", 0xA0},
    {"Brazil", "BR", 0xA2},
    {"Dominica", "DO", 0xA3},
    {"Saint Lucia", "LC", 0xA4},
    {"Burkina Faso", "BF", 0xD0},
    {"Botswana", "BW", 0xD1},
    {"Zaire", "ZR", 0xD2},
    {"Hungary", "HU", 0xE0},
    {"Iraq", "IQ", 0xE1},
    {"Monaco", "MC", 0xE2},
    {"Azerbaijan", "AZ", 0xE3},
    {"Uzbekistan", "UZ", 0xE4},
    {"Myanmar", "MM", 0xF0},
    {"Brunei", "BN", 0xF1},
    {"Maldives", "MV", 0xF2},
    {"Yemen", "YE", 0xF3},
};

const static Country eccCountriesC[] = {
    {"Canada", "CA", 0xA1},
    {"Bermuda", "BM", 0xA2},
    {"Chile", "CL", 0xA3},
    {"El Salvador", "SV", 0xA4},
    {"Saint Vincent", "VC", 0xA5},
    {"Congo", "CG", 0xD0},
    {"Comoros", "KM", 0xD1},
    {"Cote d'Ivoire", "CI", 0xD2},
    {"Sudan", "SD", 0xD3},
    {"Malta", "MT", 0xE0},
    {"United King.", "GB", 0xE1},
    {"Lithunia", "LT", 0xE2},
    {"Lithuania", "LT", 0xE2},
    {"Croatia", "HRV", 0xE3},
    {"Georgia", "GE", 0xE4},
    {"China", "CN", 0xF0},
    {"Sri Lanka", "LK", 0xF1},
    {"Indonesia", "ID", 0xF2},
};

const static Country eccCountriesD[] = {
    {"United States", "US", 0xA0},
    {"Grenada", "GD", 0xA3},
    {"Haiti", "HT", 0xA4},
    {"Togo", "TG", 0xD0},
    {"Tanzania", "TZ", 0xD1},
    {"Zanzibar", "TZ", 0xD2},
    {"Germany", "DE", 0xE0},
    {"Libya", "LY", 0xE1},
    {"Libya", "LY", 0xE1},
    {"Kazakhstan", "KZ", 0xE3},
    {"North Korea", "KP", 0xF0},
    {"Taiwan", "TW", 0xF1},
    {"Emirates", "AE", 0xF2},
};

const static Country eccCountriesE[] = {
    {"United States", "US", 0xA0},
    {"Guadeloupe", "GP", 0xA2},
    {"Turks&Caicos", "TC", 0xA3},
    {"Venezuela", "VE", 0xA4},
    {"Benin", "BJ", 0xD0},
    {"Ethiopia", "ET", 0xD1},
    {"Zambia", "ZM", 0xD2},
    {"Canary Isl.", "ES", 0xE0},
    {"Romania", "RO", 0xE1},
    {"Spain", "ES", 0xE2},
    {"Sweden", "SE", 0xE3},
    {"Turkmenistan", "TM", 0xE4},
    {"Bahrain", "BH", 0xF0},
    {"South Korea", "KR", 0xF1},
    {"Nepal", "NP", 0xF2},
    {"Micronesia", "FM", 0xF3},
};

const static Country eccCountriesF[] = {
    {"Greenland", "GL", 0xA1},
    {"Bahamas", "BS", 0xA2},
    {"Guyana", "GY", 0xA3},
    {"Mexico", "MX", 0xA4},
    {"Virgin Isl.", "VG", 0xA5},
    {"St Pierre", "PM", 0xA6},
    {"Malawi", "MW", 0xD0},
    {"Nigeria", "NG", 0xD1},
    {"Egypt", "EG", 0xE0},
    {"France", "FR", 0xE1},
    {"Norway", "NO", 0xE2},
    {"Belarus", "BLR", 0xE3},
    {"Belarus", "BY", 0xE3},
    {"Bosnia", "BIH", 0xE4},
    {"Malaysia", "MY", 0xF0},
    {"Hong Kong", "HK", 0xF1},
    {"Vanuatu", "VU", 0xF2},
    {"Mongolia", "MN", 0xF3},
};

const Country *eccLUT[] = {
    eccCountries1,
    eccCountries2,
    eccCountries3,
    eccCountries4,
    eccCountries5,
    eccCountries6,
    eccCountries7,
    eccCountries8,
    eccCountries9,
    eccCountriesA,
    eccCountriesB,
    eccCountriesC,
    eccCountriesD,
    eccCountriesE,
    eccCountriesF};

const size_t eccLUTLength[15]{
    sizeof(eccCountries1) / sizeof(eccCountries1[0]),
    sizeof(eccCountries2) / sizeof(eccCountries2[0]),
    sizeof(eccCountries3) / sizeof(eccCountries3[0]),
    sizeof(eccCountries4) / sizeof(eccCountries4[0]),
    sizeof(eccCountries5) / sizeof(eccCountries5[0]),
    sizeof(eccCountries6) / sizeof(eccCountries6[0]),
    sizeof(eccCountries7) / sizeof(eccCountries7[0]),
    sizeof(eccCountries8) / sizeof(eccCountries8[0]),
    sizeof(eccCountries9) / sizeof(eccCountries9[0]),
    sizeof(eccCountriesA) / sizeof(eccCountriesA[0]),
    sizeof(eccCountriesB) / sizeof(eccCountriesB[0]),
    sizeof(eccCountriesC) / sizeof(eccCountriesC[0]),
    sizeof(eccCountriesD) / sizeof(eccCountriesD[0]),
    sizeof(eccCountriesE) / sizeof(eccCountriesE[0]),
    sizeof(eccCountriesF) / sizeof(eccCountriesF[0]),
};