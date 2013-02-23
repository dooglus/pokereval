#!/usr/bin/python

import sys, string, posix, random, os.path, json
from BitcoinVideoCasino import BitcoinVideoCasino

# account_key = '2a12029392838293aaaaa77a7373737a'
account_key = None

# 1 through 5
credits_per_game = 1

# The size of 1 credit in this game, in Satoshis.
# This value can be 100000 (0.001 BTC), 500000 (0.005 BTC), or 1000000 (0.01 BTC).
credit_btc_value = 100000

########################################################################
# end of user-configable variables
########################################################################

hand_names = ['Nothing',
              'One Pair (Jacks or Better)',
              'Two Pair',
              '3 of a Kind',
              'Straight',
              'Flush',
              'Full House',
              '4 of a Kind',
              'Straight Flush',
              'Royal Flush']

credits_bet = credits_won = 0

referral_code = '2850842937'

bvc = BitcoinVideoCasino(referral_code, account_key)

if (not account_key):
    account_key = bvc.account_new()
    print "new account key: %s" % (account_key,)

game_type = bvc.PAYTABLE_JACKS_OR_BETTER
server_seed_hash = bvc.videopoker_reseed()['server_seed_hash']
if (not server_seed_hash):
    print "Bad key?"
    sys.exit(1)

while True:
    client_seed = random.randint(0, 9999999999)

    credits_bet += credits_per_game
    deal = bvc.videopoker_deal(credits_per_game, game_type, server_seed_hash, client_seed, credit_btc_value)

    if deal.has_key('error'):
        print "Error: %s" % deal['error']
        sys.exit(1)

    # Your initial 5 cards for this game.
    cards = deal['cards']

    # The primary unique ID for this game.  This ID will need to be included in further game actions.
    game_id = deal['game_id']

    # The current value of the progressive jackpot.
    progressive_jackpot = deal['progressive_jackpot']

    # The initial hand evaluation of your dealt cards. This value depends on the paytable in play
    hand_eval = deal['hand_eval']

    # We only qualify for the progressive pot if we bet 5 credits
    if (credits_per_game == 5):
        progressive_jackpot = progressive_jackpot / 10000.0 + 4000
    else:
        progressive_jackpot = 4000

    cards = reduce((lambda a, b: a+' '+b), cards)
    try:
        print "game_id: %s; hand: %s" % (game_id, hand_names[hand_eval])
    except:
        print "oops!"

    command = "./jacks %s %s" % (progressive_jackpot, cards)
    result = posix.popen(command).readlines()
    for line in result:
        print "  : %s" % (line[:-1],)
    holds = string.split(result[-1][:-1])[0]

    hold = bvc.videopoker_hold(game_id, holds)
	
    # An array indicating the new cards that you were dealt to replace the cards that were not held.
    cards = hold['cards']

    # The prize you won in credits.
    prize = hold['prize']

    # This will be the server seed to the next game of video poker, so that you do not need to issue /videopoker/reseed after every game.
    server_seed_hash = hold['server_seed_hash']

    # Your balance after your prize has been added to your account.
    intbalance = hold['intbalance']

    # The server's evaluation of your resulting hand
    hand_eval = hold['hand_eval']

    credits_won += prize

    print

    if (cards):
        cards = reduce((lambda a, b: a+' '+b), cards)
        sys.stdout.write("new cards: %s; " % (cards,))

    sys.stdout.write("hand: %s; prize: %s; credits bet: %s; credits won: %s; return: %.4f%%; " %
                     (hand_names[hand_eval], prize, credits_bet, credits_won, credits_won*100.0/credits_bet))

    balance = intbalance/1e8
    print "balance: %s\n" % (balance,)

    if (intbalance < credit_btc_value * credits_per_game):
        print
        sys.exit(0)

    # sys.stdout.write('[ret] ')
    # sys.stdin.readline()

    if (os.path.exists('stop')):
        sys.exit(0)
