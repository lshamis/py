from alephzero_bindings import *
import asyncio
import threading
import types


class aio_sub:
    def __init__(self, shm, init_, iter_, loop=None):
        ns = types.SimpleNamespace()
        ns.loop = loop or asyncio.get_event_loop()
        ns.q = asyncio.Queue(1)
        ns.cv = threading.Condition()
        ns.closing = False
        
        # Note: To prevent cyclic dependencies, `callback` is NOT owned by self.
        def callback(pkt):
            ns.loop.call_soon_threadsafe(lambda: asyncio.ensure_future(ns.q.put(pkt), loop=ns.loop))
            ns.cv.acquire()
            ns.cv.wait()
            ns.cv.release()

        self._ns = ns
        self._sub = Subscriber(shm, init_, iter_, callback)

    def __aiter__(self):
        return self
    
    async def __anext__(self):
        pkt = await self._ns.q.get()
        self._ns.cv.acquire()
        self._ns.cv.notify()
        self._ns.cv.release()
        return pkt

