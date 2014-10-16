package com.easydlna.dlna.service;

public interface IDMRService extends android.os.IInterface {
    /** Local-side IPC implementation stub class. */
    public static abstract class Stub extends android.os.Binder implements
            com.easydlna.dlna.service.IDMRService {
        private static final java.lang.String DESCRIPTOR = "com.easydlna.dlna.service.IDMRService";

        /** Construct the stub at attach it to the interface. */
        public Stub() {
            this.attachInterface(this, DESCRIPTOR);
        }

        /**
         * Cast an IBinder object into an com.easydlna.dlna.service.IDMRService
         * interface, generating a proxy if needed.
         */
        public static com.easydlna.dlna.service.IDMRService asInterface(
                android.os.IBinder obj) {
            if ((obj == null)) {
                return null;
            }
            android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
            if (((iin != null) && (iin instanceof com.easydlna.dlna.service.IDMRService))) {
                return ((com.easydlna.dlna.service.IDMRService) iin);
            }
            return new com.easydlna.dlna.service.IDMRService.Stub.Proxy(obj);
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
            case TRANSACTION_start_stop_DMR: {
                data.enforceInterface(DESCRIPTOR);
                boolean _arg0;
                _arg0 = (0 != data.readInt());
                this.start_stop_DMR(_arg0);
                reply.writeNoException();
                return true;
            }
            case TRANSACTION_addDMRCallback: {
                data.enforceInterface(DESCRIPTOR);
                com.easydlna.dlna.service.IDMRCallback _arg0;
                _arg0 = com.easydlna.dlna.service.IDMRCallback.Stub
                        .asInterface(data.readStrongBinder());
                this.addDMRCallback(_arg0);
                reply.writeNoException();
                return true;
            }
            case TRANSACTION_removeDMRCallback: {
                data.enforceInterface(DESCRIPTOR);
                com.easydlna.dlna.service.IDMRCallback _arg0;
                _arg0 = com.easydlna.dlna.service.IDMRCallback.Stub
                        .asInterface(data.readStrongBinder());
                this.removeDMRCallback(_arg0);
                reply.writeNoException();
                return true;
            }
            case TRANSACTION_setRenderStatus: {
                data.enforceInterface(DESCRIPTOR);
                com.easydlna.dlna.service.RenderStatus _arg0;
                if ((0 != data.readInt())) {
                    _arg0 = com.easydlna.dlna.service.RenderStatus.CREATOR
                            .createFromParcel(data);
                } else {
                    _arg0 = null;
                }
                int _result = this.setRenderStatus(_arg0);
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_setRenderFriendlyName: {
                data.enforceInterface(DESCRIPTOR);
                java.lang.String _arg0;
                _arg0 = data.readString();
                boolean _arg1;
                _arg1 = (0 != data.readInt());
                int _result = this.setRenderFriendlyName(_arg0, _arg1);
                reply.writeNoException();
                reply.writeInt(_result);
                return true;
            }
            case TRANSACTION_getRenderFriendlyName: {
                data.enforceInterface(DESCRIPTOR);
                java.lang.String _result = this.getRenderFriendlyName();
                reply.writeNoException();
                reply.writeString(_result);
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
                com.easydlna.dlna.service.IDMRService {
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
            public void start_stop_DMR(boolean bStart)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeInt(((bStart) ? (1) : (0)));
                    mRemote.transact(Stub.TRANSACTION_start_stop_DMR, _data,
                            _reply, 0);
                    _reply.readException();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }

            @Override
            public void addDMRCallback(
                    com.easydlna.dlna.service.IDMRCallback callback)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeStrongBinder((((callback != null)) ? (callback
                            .asBinder()) : (null)));
                    mRemote.transact(Stub.TRANSACTION_addDMRCallback, _data,
                            _reply, 0);
                    _reply.readException();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }

            @Override
            public void removeDMRCallback(
                    com.easydlna.dlna.service.IDMRCallback callback)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeStrongBinder((((callback != null)) ? (callback
                            .asBinder()) : (null)));
                    mRemote.transact(Stub.TRANSACTION_removeDMRCallback, _data,
                            _reply, 0);
                    _reply.readException();
                } finally {
                    _reply.recycle();
                    _data.recycle();
                }
            }

            @Override
            public int setRenderStatus(
                    com.easydlna.dlna.service.RenderStatus renderStatus)
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    if ((renderStatus != null)) {
                        _data.writeInt(1);
                        renderStatus.writeToParcel(_data, 0);
                    } else {
                        _data.writeInt(0);
                    }
                    mRemote.transact(Stub.TRANSACTION_setRenderStatus, _data,
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
            public int setRenderFriendlyName(java.lang.String name,
                    boolean reStartRender) throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                int _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    _data.writeString(name);
                    _data.writeInt(((reStartRender) ? (1) : (0)));
                    mRemote.transact(Stub.TRANSACTION_setRenderFriendlyName,
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
            public java.lang.String getRenderFriendlyName()
                    throws android.os.RemoteException {
                android.os.Parcel _data = android.os.Parcel.obtain();
                android.os.Parcel _reply = android.os.Parcel.obtain();
                java.lang.String _result;
                try {
                    _data.writeInterfaceToken(DESCRIPTOR);
                    mRemote.transact(Stub.TRANSACTION_getRenderFriendlyName,
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

        static final int TRANSACTION_start_stop_DMR = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
        static final int TRANSACTION_addDMRCallback = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
        static final int TRANSACTION_removeDMRCallback = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
        static final int TRANSACTION_setRenderStatus = (android.os.IBinder.FIRST_CALL_TRANSACTION + 3);
        static final int TRANSACTION_setRenderFriendlyName = (android.os.IBinder.FIRST_CALL_TRANSACTION + 4);
        static final int TRANSACTION_getRenderFriendlyName = (android.os.IBinder.FIRST_CALL_TRANSACTION + 5);
        static final int TRANSACTION_setAutoStartup = (android.os.IBinder.FIRST_CALL_TRANSACTION + 6);
        static final int TRANSACTION_sendBroadcast = (android.os.IBinder.FIRST_CALL_TRANSACTION + 7);
    }

    public void start_stop_DMR(boolean bStart)
            throws android.os.RemoteException;

    public void addDMRCallback(com.easydlna.dlna.service.IDMRCallback callback)
            throws android.os.RemoteException;

    public void removeDMRCallback(
            com.easydlna.dlna.service.IDMRCallback callback)
            throws android.os.RemoteException;

    public int setRenderStatus(
            com.easydlna.dlna.service.RenderStatus renderStatus)
            throws android.os.RemoteException;

    public int setRenderFriendlyName(java.lang.String name,
            boolean reStartRender) throws android.os.RemoteException;

    public java.lang.String getRenderFriendlyName()
            throws android.os.RemoteException;

    public boolean setAutoStartup(boolean bAutoStartup)
            throws android.os.RemoteException;

    public void sendBroadcast(int timeout) throws android.os.RemoteException;
}
