package com.easydlna.dlna.service;

public interface IDMSService extends android.os.IInterface {
    /** Local-side IPC implementation stub class. */
    public static abstract class Stub extends android.os.Binder implements
            com.easydlna.dlna.service.IDMSService {
        private static final java.lang.String DESCRIPTOR = "com.easydlna.dlna.service.IDMSService";

        /** Construct the stub at attach it to the interface. */
        public Stub() {
            this.attachInterface(this, DESCRIPTOR);
        }

        /**
         * Cast an IBinder object into an com.easydlna.dlna.service.IDMSService
         * interface, generating a proxy if needed.
         */
        public static com.easydlna.dlna.service.IDMSService asInterface(
                android.os.IBinder obj) {
            if ((obj == null)) {
                return null;
            }
            android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
            if (((iin != null) && (iin instanceof com.easydlna.dlna.service.IDMSService))) {
                return ((com.easydlna.dlna.service.IDMSService) iin);
            }
            return new com.easydlna.dlna.service.IDMSService.Stub.Proxy(obj);
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
            case TRANSACTION_start_stop_DMS: {
                data.enforceInterface(DESCRIPTOR);
                boolean _arg0;
                _arg0 = (0 != data.readInt());
                int _result = this.start_stop_DMS(_arg0);
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_setServerFriendlyName: {
                data.enforceInterface(DESCRIPTOR);
                java.lang.String _arg0;
                _arg0 = data.readString();
                boolean _arg1;
                _arg1 = (0 != data.readInt());
                int _result = this.setServerFriendlyName(_arg0, _arg1);
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_getServerFriendlyName: {
                data.enforceInterface(DESCRIPTOR);
                java.lang.String _result = this.getServerFriendlyName();
                reply.writeNoException();
                reply.writeString(_result);
                return true;
            }
            case TRANSACTION_getSharedType: {
                data.enforceInterface(DESCRIPTOR);
                int _result = this.getSharedType();
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_addSharedContents: {
                data.enforceInterface(DESCRIPTOR);
                int _arg0;
                _arg0 = data.readInt();
                int _result = this.addSharedContents(_arg0);
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_removeSharedContents: {
                data.enforceInterface(DESCRIPTOR);
                int _arg0;
                _arg0 = data.readInt();
                int _result = this.removeSharedContents(_arg0);
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_setSharedType: {
                data.enforceInterface(DESCRIPTOR);
                int _arg0;
                _arg0 = data.readInt();
                this.setSharedType(_arg0);
                reply.writeNoException();
                return true;
            }
            case TRANSACTION_setAutoStartup: {
                data.enforceInterface(DESCRIPTOR);
                boolean _arg0;
                _arg0 = (0 != data.readInt());
                boolean _result = this.setAutoStartup(_arg0);
                reply.writeNoException();
                reply.writeInt(((_result) ? (1) : (0)));
                return true;
            }
            case TRANSACTION_getAutoStartup: {
                data.enforceInterface(DESCRIPTOR);
                boolean _result = this.getAutoStartup();
                reply.writeNoException();
                reply.writeInt(((_result) ? (1) : (0)));
                return true;
            }
            case TRANSACTION_isStarted: {
                data.enforceInterface(DESCRIPTOR);
                boolean _result = this.isStarted();
                reply.writeNoException();
                reply.writeInt(((_result) ? (1) : (0)));
                return true;
            }
            case TRANSACTION_sendBroadcast: {
                data.enforceInterface(DESCRIPTOR);
                int _arg0;
                _arg0 = data.readInt();
                this.sendBroadcast(_arg0);
                reply.writeNoException();
                return true;
            }
            }
            return super.onTransact(code, data, reply, flags);
        }

        private static class Proxy implements
                com.easydlna.dlna.service.IDMSService {
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
            public int start_stop_DMS(boolean bStart)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(((bStart) ? (1) : (0)));
                    mRemote.transact(Stub.TRANSACTION_start_stop_DMS, _data,
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
            public int setServerFriendlyName(java.lang.String name,
                    boolean reStartServer) throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeString(name);
                    _data.writeInt(((reStartServer) ? (1) : (0)));
                    mRemote.transact(Stub.TRANSACTION_setServerFriendlyName,
                            _data, _reply, 0);
                    _reply.readException();
                    _result = _reply.readInt();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public java.lang.String getServerFriendlyName()
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                java.lang.String _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    mRemote.transact(Stub.TRANSACTION_getServerFriendlyName,
                            _data, _reply, 0);
                    _reply.readException();
                    _result = _reply.readString();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public int getSharedType() throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    mRemote.transact(Stub.TRANSACTION_getSharedType, _data,
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
            public int addSharedContents(int type)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(type);
                    mRemote.transact(Stub.TRANSACTION_addSharedContents, _data,
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
            public int removeSharedContents(int type)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(type);
                    mRemote.transact(Stub.TRANSACTION_removeSharedContents,
                            _data, _reply, 0);
                    _reply.readException();
                    _result = _reply.readInt();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public void setSharedType(int type)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(type);
                    mRemote.transact(Stub.TRANSACTION_setSharedType, _data,
                            _reply, 0);
                    _reply.readException();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }

            @Override
            public boolean setAutoStartup(boolean bAutoStartup)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                boolean _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(((bAutoStartup) ? (1) : (0)));
                    mRemote.transact(Stub.TRANSACTION_setAutoStartup, _data,
                            _reply, 0);
                    _reply.readException();
                    _result = (0 != _reply.readInt());
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public boolean getAutoStartup() throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                boolean _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    mRemote.transact(Stub.TRANSACTION_getAutoStartup, _data,
                            _reply, 0);
                    _reply.readException();
                    _result = (0 != _reply.readInt());
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public boolean isStarted() throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                boolean _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    mRemote.transact(Stub.TRANSACTION_isStarted, _data, _reply,
                            0);
                    _reply.readException();
                    _result = (0 != _reply.readInt());
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
                return _result;
            }

            @Override
            public void sendBroadcast(int timeout)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(timeout);
                    mRemote.transact(Stub.TRANSACTION_sendBroadcast, _data,
                            _reply, 0);
                    _reply.readException();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }
        }

        static final int TRANSACTION_start_stop_DMS = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
        static final int TRANSACTION_setServerFriendlyName = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
        static final int TRANSACTION_getServerFriendlyName = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
        static final int TRANSACTION_getSharedType = (android.os.IBinder.FIRST_CALL_TRANSACTION + 3);
        static final int TRANSACTION_addSharedContents = (android.os.IBinder.FIRST_CALL_TRANSACTION + 4);
        static final int TRANSACTION_removeSharedContents = (android.os.IBinder.FIRST_CALL_TRANSACTION + 5);
        static final int TRANSACTION_setSharedType = (android.os.IBinder.FIRST_CALL_TRANSACTION + 6);
        static final int TRANSACTION_setAutoStartup = (android.os.IBinder.FIRST_CALL_TRANSACTION + 7);
        static final int TRANSACTION_getAutoStartup = (android.os.IBinder.FIRST_CALL_TRANSACTION + 8);
        static final int TRANSACTION_isStarted = (android.os.IBinder.FIRST_CALL_TRANSACTION + 9);
        static final int TRANSACTION_sendBroadcast = (android.os.IBinder.FIRST_CALL_TRANSACTION + 10);
    }

    public int start_stop_DMS(boolean bStart) throws android.os.RemoteException;

    public int setServerFriendlyName(java.lang.String name,
            boolean reStartServer) throws android.os.RemoteException;

    public java.lang.String getServerFriendlyName()
            throws android.os.RemoteException;

    public int getSharedType() throws android.os.RemoteException;

    public int addSharedContents(int type) throws android.os.RemoteException;

    public int removeSharedContents(int type) throws android.os.RemoteException;

    public void setSharedType(int type) throws android.os.RemoteException;

    public boolean setAutoStartup(boolean bAutoStartup)
            throws android.os.RemoteException;

    public boolean getAutoStartup() throws android.os.RemoteException;

    public boolean isStarted() throws android.os.RemoteException;

    public void sendBroadcast(int timeout) throws android.os.RemoteException;
}
