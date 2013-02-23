import urllib, json

class BitcoinVideoCasino:
    PAYTABLE_JACKS_OR_BETTER = 0
    TENS_OR_BETTER = 1
    BONUS_POKER = 2
    DOUBLE_BONUS_POKER = 3
    DOUBLE_DOUBLE_BONUS_POKER = 4
    DEUCES_WILD_POKER = 5
    BONUS_DELUXE_POKER = 6

    def __init__(self, referral_code = None, key = None):
        self.referral_code = referral_code
        self.key = key
        self.site = 'https://bitcoinvideocasino.com/'

    def query_without_key(self, api, args = {}):
        if args:
            url = "%s%s?%s" % (self.site, api, urllib.urlencode(args))
        else:
            url = "%s%s" % (self.site, api)
        # print "url: %s" % url
        ret = urllib.urlopen(url).read()
        # print "ret: %s" % ret
        return json.loads(ret)

    def query(self, api, args = {}):
        args['account_key'] = self.key
        return self.query_without_key(api, args)

    def account_new(self):
        args = {}
        if self.referral_code:
            args['r'] = self.referral_code
        ret = self.query_without_key("account/new", args)
        self.key = ret['account_key']
        return self.key

    # VIDEO POKER API

    def videopoker_reseed(self):
        return self.query("videopoker/reseed")

    def videopoker_deal(self, bet_size, paytable, server_seed_hash, client_seed, credit_btc_value):
        return self.query("videopoker/deal", {'bet_size': bet_size,
                                              'paytable': paytable,
                                              'server_seed_hash': server_seed_hash,
                                              'client_seed': client_seed,
                                              'credit_btc_value': credit_btc_value})

    def videopoker_hold(self, game_id, holds):
        return self.query("videopoker/hold", {'game_id': game_id,
                                              'holds': holds})
