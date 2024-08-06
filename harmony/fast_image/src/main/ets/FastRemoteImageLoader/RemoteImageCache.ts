import { RemoteImageSource } from './RemoteImageSource';

export class RemoteImageCache<T> {
  protected data: Map<string, T>;
  protected maxSize: number;

  constructor(maxSize: number) {
    this.data = new Map<string, T>();
    this.maxSize = maxSize;
  }

  set(key: string, value: T): void {
    this.data.set(key, value);

    // Check if memoryCache size exceeds maxSize, and if so, remove the oldest entry
    if (this.data.size > this.maxSize) {
      const oldestKey = this.data.keys()[0];
      this.remove(oldestKey);
    }
  }

  get(key: string): T | undefined {
    return this.data.get(key);
  }

  has(key: string): boolean {
    return this.data.has(key);
  }

  remove(key: string): void {
    this.data.delete(key);
  }

  clear(): void {
    this.data.clear();
  }

  size(): number {
    return this.data.size;
  }

  keys(): IterableIterator<string> {
    return this.data.keys();
  }

  values(): IterableIterator<T> {
    return this.data.values();
  }
  entries(): IterableIterator<[string, T]> {
    return this.data.entries();
  }
}

export class RemoteImageMemoryCache extends RemoteImageCache<RemoteImageSource> {
  public memoryCacheClear():boolean{
    let PromiseArr = []
    for (const entry of this.data.entries()){
      PromiseArr.push(entry[1].release().then(()=>{this.data.delete(entry[0])}))
    }
    Promise.all(PromiseArr).then(
    ).catch((err)=>{
      return false;
    })
    return true;
  }
};
