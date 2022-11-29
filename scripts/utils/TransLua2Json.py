import json
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--src')
parser.add_argument('--dst')
parser.add_argument('--idx')

if __name__ == "__main__":
    args = parser.parse_args()
    data = {'id': int(args.idx)}
    with open(args.src, 'r') as f:
        strs = f.read()
    data.update({'script': strs})
    f = open(args.dst, 'w')
    json.dump(data, f)
