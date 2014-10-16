package com.easydlna.dlna.service;

public interface IDMRCallback extends android.os.IInterface {
    /** Local-side IPC implementation stub class. */
    public static abstract class Stub extends android.os.Binder implements
            com.easydlna.dlna.service.IDMRCallback {
        private static final java.lang.String DESCRIPTOR = "com.easydlna.dlna.service.IDMRCallback";

        /** Construct the stub at attach it to the interface. */
        public Stub() {
            this.attachInterface(this, DESCRIPTOR);
        }

        /**
         * Cast an IBinder object into an com.easydlna.dlna.service.IDMRCallback
         * interface, generating a proxy if needed.
         */
        public static com.easydlna.dlna.service.IDMRCallback asInterface(
                android.os.IBinder obj) {
            if ((obj == null)) {
                return null;
            }
            android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
            if (((iin != null) && (iin instanceof com.easydlna.dlna.service.IDMRCallback))) {
                return ((com.easydlna.dlna.service.IDMRCallback) iin);
            }
            return new com.easydlna.dlna.service.IDMRCallback.Stub.Proxy(obj);
        }

        @Override
        public android.os.IBinder asBinder() {
            return this;
        }

        @Override
        public boolean onTransact(int code, android.os.Parcel data,
                android.os.Parcel reply, int flags)
                throws android.os.RemoteException {
            switch (code) {
            case INTERFACE_TRANSACTION: {
                reply.writeString(DESCRIPTOR);
                return true;
            }
            case TRANSACTION_setDataSource: {
                data.enforceInterface(DESCRIPTOR);
                java.lang.String _arg0;
                _arg0 = data.readString();
                int _result = this.setDataSource(_arg0);
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_stop: {
                data.enforceInterface(DESCRIPTOR);
                int _result = this.stop();
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_play: {
                data.enforceInterface(DESCRIPTOR);
                int _result = this.play();
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_pause: {
                data.enforceInterface(DESCRIPTOR);
                int _result = this.pause();
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_seek: {
                data.enforceInterface(DESCRIPTOR);
                int _arg0;
                _arg0 = data.readInt();
                int _result = this.seek(_arg0);
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_getPosition: {
                data.enforceInterface(DESCRIPTOR);
                int _result = this.getPosition();
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            }
            return super.onTransact(code, data, reply, flags);
        }

        private static class Proxy implements
                com.easydlna.dlna.service.IDMRCallback {
            private android.os.IBinder mRemote;

            Proxy(android.os.IBinder remote) {
                mRemote = remote;
            }

            @Override
            public android.os.IBinder asBinder() {
                return mRemote;
            }

            public java.lang.String getInterfaceDescriptor() {
                return DESCRIPTOR;
            }

            @Override
            public int setDataSource(java.lang.String urlSource)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeString(urlSource);
                    mRemote.transact(Stub.TRANSACTION_setDataSource, _data,
                            _reply, 0);
                    _reply.readException();
                    _result = _reply.readInt();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public int stop() throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    mRemote.transact(Stub.TRANSACTION_stop, _data, _reply, 0);
                    _reply.readException();
                    _result = _reply.readInt();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public int play() throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    mRemote.transact(Stub.TRANSACTION_play, _data, _reply, 0);
                    _reply.readException();
                    _result = _reply.readInt();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public int pause() throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    mRemote.transact(Stub.TRANSACTION_pause, _data, _reply, 0);
                    _reply.readException();
                    _result = _reply.readInt();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public int seek(int seekTo) throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(seekTo);
                    mRemote.transact(Stub.TRANSACTION_seek, _data, _reply, 0);
                    _reply.readException();
                    _result = _reply.readInt();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public int getPosition() throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    mRemote.transact(Stub.TRANSACTION_getPosition, _data,
                            _reply, 0);
                    _reply.readException();
                    _result = _reply.readInt();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }
        }

        static final int TRANSACTION_setDataSource = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
        static final int TRANSACTION_stop = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
        static final int TRANSACTION_play = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
        static final int TRANSACTION_pause = (android.os.IBinder.FIRST_CALL_TRANSACTION + 3);
        static final int TRANSACTION_seek = (android.os.IBinder.FIRST_CALL_TRANSACTION + 4);
        static final int TRANSACTION_getPosition = (android.os.IBinder.FIRST_CALL_TRANSACTION + 5);
    }

    public int setDataSource(java.lang.String urlSource)
            throws android.os.RemoteException;

    public int stop() throws android.os.RemoteException;

    public int play() throws android.os.RemoteException;

    public int pause() throws android.os.RemoteException;

    public int seek(int seekTo) throws android.os.RemoteException;

    public int getPosition() throws android.os.RemoteException;
}
