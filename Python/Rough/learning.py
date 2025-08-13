from urllib.parse import urlparse

url = "https://www.lightnovelworld.co/novel/im-an-infinite-regressor-but-ive-got-stories-to-tell-1684/chapter-1"
parser = urlparse(url)

paths = parser.path.strip('/').split('/')
slug = paths[1]
print(paths)
max_chapters = 3

base_url = f"https://www.lightnovelworld.co/novel/{slug}/"

chapters = []
for i in range(1, max_chapters+1):
    new_url = base_url + f"chapter-{i}"
    print(new_url)
    chapter = {
        'id': i,
        'url': new_url,
        'title': f"chapter-{i}",
        'volume':1
    }
    chapters.append(chapter)

for chap in chapters:
    print(chap)
    
